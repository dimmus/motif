#!/usr/bin/env python3
"""X11 proxy that delays the traffic and counts round trips.

usage: xproxy.py PORT SERVER_SOCKET DELAY_MS STATS_FILE

Listens on 127.0.0.1:PORT (display :PORT-6000 over TCP) and forwards each
connection to the X server's Unix socket SERVER_SOCKET (for example
/tmp/.X11-unix/X1), holding the data DELAY_MS in each direction: the
round trip becomes about twice DELAY_MS.  When a connection closes, a
JSON line is appended to STATS_FILE with the number of requests, of
replies (the round trips: one per request that has a reply), events and
errors, and the requests by major opcode.  See latency.sh.
"""
import asyncio
import json
import struct
import sys
import time


def requests(stats, state):
    """Parser of the client's side: the setup, then requests."""
    buf = bytearray()

    def parse(data):
        buf.extend(data)
        while True:
            setup = "le" not in state
            if setup:
                if len(buf) < 12:
                    return
                order = "<" if buf[0] == 0x6C else ">"
                n, d = struct.unpack(order + "HH", bytes(buf[6:10]))
                size = 12 + ((n + 3) & ~3) + ((d + 3) & ~3)
            else:
                if len(buf) < 4:
                    return
                order = "<" if state["le"] else ">"
                (length,) = struct.unpack(order + "H", bytes(buf[2:4]))
                if length == 0:  # BIG-REQUESTS
                    if len(buf) < 8:
                        return
                    (length,) = struct.unpack(order + "I", bytes(buf[4:8]))
                size = 4 * length
            if len(buf) < size:
                return
            if setup:
                state["le"] = order == "<"
            else:
                stats["requests"] += 1
                op = str(buf[0])
                stats["opcodes"][op] = stats["opcodes"].get(op, 0) + 1
            del buf[:size]

    return parse


def replies(stats, state):
    """Parser of the server's side: the setup reply, then 32-byte units."""
    buf = bytearray()
    setup = [True]

    def parse(data):
        buf.extend(data)
        while True:
            order = "<" if state.get("le", True) else ">"
            if setup[0]:
                if len(buf) < 8:
                    return
                (n,) = struct.unpack(order + "H", bytes(buf[6:8]))
                size = 8 + 4 * n
            else:
                if len(buf) < 32:
                    return
                kind = buf[0] & 0x7F
                size = 32
                if kind in (1, 35):  # reply, GenericEvent
                    (n,) = struct.unpack(order + "I", bytes(buf[4:8]))
                    size += 4 * n
            if len(buf) < size:
                return
            if not setup[0]:
                key = {0: "errors", 1: "replies"}.get(kind, "events")
                stats[key] += 1
            setup[0] = False
            del buf[:size]

    return parse


async def pump(reader, writer, parse, delay):
    queue = asyncio.Queue()

    async def send():
        while True:
            item = await queue.get()
            if item is None:
                break
            due, data = item
            wait = due - time.monotonic()
            if wait > 0:
                await asyncio.sleep(wait)
            writer.write(data)
            await writer.drain()
        writer.close()

    sender = asyncio.ensure_future(send())
    try:
        while True:
            data = await reader.read(65536)
            if not data:
                break
            parse(data)
            queue.put_nowait((time.monotonic() + delay, data))
    except ConnectionError:
        pass
    queue.put_nowait(None)
    try:
        await sender
    except ConnectionError:
        pass


def main():
    port, server, delay, stats_file = (int(sys.argv[1]), sys.argv[2],
                                       float(sys.argv[3]) / 1000, sys.argv[4])

    async def connection(client_reader, client_writer):
        stats = {"requests": 0, "replies": 0, "events": 0, "errors": 0, "opcodes": {}}
        state = {}
        server_reader, server_writer = await asyncio.open_unix_connection(server)
        await asyncio.gather(
            pump(client_reader, server_writer, requests(stats, state), delay),
            pump(server_reader, client_writer, replies(stats, state), delay))
        with open(stats_file, "a") as f:
            f.write(json.dumps(stats) + "\n")

    async def serve():
        listener = await asyncio.start_server(connection, "127.0.0.1", port)
        async with listener:
            await listener.serve_forever()

    asyncio.run(serve())


if __name__ == "__main__":
    main()
