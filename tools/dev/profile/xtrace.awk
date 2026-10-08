# Summarise an xtrace log (xtrace -n -o LOG ...).
#
#   awk -f xtrace.awk LOG
#
# Prints the number of requests, replies, events and errors, then the
# requests that got the most replies.  xtrace marks what the client
# sends with ":<:" and what the server sends with ":>:", and numbers
# requests and replies ("000:<:0004: 24: Request(20): GetProperty ...",
# "000:>:0004:32: Reply to GetProperty: ...").
#
# Replies are not round trips: Xlib gets several replies in one wait for
# XInternAtoms or XGetWindowAttributes.  libxmbench_preload counts the
# round trips (XMBENCH_REPORT=1).

/:<:[0-9a-f][0-9a-f][0-9a-f][0-9a-f]+:/ {
	requests++
	next
}
/:>:[0-9a-f][0-9a-f][0-9a-f][0-9a-f]+:[0-9]+: Reply to / {
	replies++
	name = $0
	sub(/.*: Reply to /, "", name)
	sub(/[^A-Za-z].*/, "", name)
	by[name]++
	next
}
/:>:[0-9a-f][0-9a-f][0-9a-f][0-9a-f]+: Event / {
	events++
	next
}
/:>:[0-9a-f][0-9a-f][0-9a-f][0-9a-f]+:([0-9]+:)? *Error/ {
	errors++
	next
}
END {
	printf "requests %d\nreplies  %d\nevents   %d\nerrors   %d\n",
		requests, replies, events, errors
	print "replies by request:"
	n = 0
	for (r in by)
		list[n++] = sprintf("%7d %s", by[r], r)
	# A small insertion sort, most replies first (no gawk asort).
	for (i = 1; i < n; i++) {
		v = list[i]
		for (j = i - 1; j >= 0 && list[j] + 0 < v + 0; j--)
			list[j + 1] = list[j]
		list[j + 1] = v
	}
	for (i = 0; i < n && i < 12; i++)
		print list[i]
}
