# Motif Drawing and Rendering System: A Deep Dive into Draw.c

## Introduction

The Motif drawing system is one of the most sophisticated aspects of the Xm library, providing a comprehensive set of rendering primitives that create the distinctive 3D appearance that Motif is famous for. In this post, we'll analyze the `Draw.c` module to understand how Motif achieves its visual effects.

## The Draw.c Module Architecture

The `Draw.c` module serves as the central drawing utility for the entire Motif toolkit. It's designed with performance and flexibility in mind, providing both simple and complex drawing operations that are used by all Motif widgets.

### Key Design Principles

1. **Performance First** - Uses X11 drawing primitives optimally
2. **Memory Efficiency** - Smart allocation and reuse of drawing resources
3. **Thread Safety** - Proper locking mechanisms for multi-threaded environments
4. **Flexibility** - Support for various shadow and highlight styles

## Core Drawing Functions

### 1. Shadow Drawing System

The shadow system is the heart of Motif's 3D appearance. Let's examine the `DrawSimpleShadow` function:

```c
static void DrawSimpleShadow(Display *display,
                           Drawable d,
                           GC top_gc,
                           GC bottom_gc,
                           Position x,
                           Position y,
                           Dimension width,
                           Dimension height,
                           Dimension shadow_thick,
                           Dimension cor)
```

This function implements a sophisticated shadow algorithm that:

- **Calculates Optimal Shadow Thickness** - Automatically adjusts shadow size based on widget dimensions
- **Uses XSegments for Performance** - More efficient than XRectangles for line-based drawing
- **Implements Corner Handling** - The `cor` parameter controls corner rendering behavior
- **Supports Multiple Shadow Styles** - Different shadow types through GC manipulation

#### Shadow Thickness Calculation

```c
ASSIGN_MIN(shadow_thick, (width >> 1));
ASSIGN_MIN(shadow_thick, (height >> 1));
```

This ensures shadows never exceed half the widget dimensions, maintaining visual balance.

#### Memory Management Strategy

```c
static XSegment * segms = NULL;
static int segm_count = 0;

if (segm_count < shadow_thick) {
    segms = (XSegment *) XtRealloc((char*)segms,
                                   sizeof (XSegment) * (size2 << 1));
    segm_count = shadow_thick;
}
```

The function uses static memory allocation with intelligent reallocation, avoiding repeated memory allocations for similar shadow operations.

### 2. Public Shadow Drawing API

The `XmeDrawShadows` function provides the public interface for shadow drawing:

```c
void XmeDrawShadows(Display *display, Drawable d,
                   GC top_gc, GC bottom_gc,
                   Position x, Position y,
                   Dimension width, Dimension height, 
                   Dimension shad_thick,
                   unsigned int shad_type)
```

#### Shadow Types Supported

- **XmSHADOW_IN** - Inset shadows (pressed button appearance)
- **XmSHADOW_OUT** - Outset shadows (raised button appearance)
- **XmSHADOW_ETCHED_IN** - Double-line inset shadows
- **XmSHADOW_ETCHED_OUT** - Double-line outset shadows

#### Etched Shadow Implementation

For etched shadows, the function creates a sophisticated double-shadow effect:

```c
if ((shad_type == XmSHADOW_ETCHED_IN ||
     shad_type == XmSHADOW_ETCHED_OUT) && (shad_thick != 1)) {
    // Draw outer shadow
    DrawSimpleShadow(display, d, top_gc, bottom_gc, x, y,
                     width, height, shad_thick/2, 1);
    // Draw inner shadow with offset
    DrawSimpleShadow(display, d, bottom_gc, top_gc,
                     x + shad_thick/2, y + shad_thick/2,
                     width - (shad_thick/2)*2,
                     height - (shad_thick/2)*2, shad_thick/2, 1);
}
```

This creates the characteristic "etched" appearance by drawing two concentric shadows with different offsets.

### 3. Border Clearing System

The `XmeClearBorder` function provides clean border management:

```c
void XmeClearBorder(Display *display, Window w,
                   Position x, Position y,
                   Dimension width, Dimension height, 
                   Dimension shadow_thick)
```

This function clears the border areas around a widget, ensuring clean rendering when shadows are redrawn or removed.

### 4. Highlight Rendering

The `XmeDrawHighlight` function creates visual feedback for user interactions:

```c
void XmeDrawHighlight(Display *display, Drawable d,
                     GC gc, Position x, Position y,
                     Dimension width, Dimension height,
                     Dimension highlight_thickness)
```

#### Highlight Implementation Strategy

The function uses a clever four-rectangle approach to draw highlights efficiently:

```c
XRectangle rect[4];
// Top border
rect[0].x = x; rect[0].y = y;
rect[0].width = width; rect[0].height = highlight_thickness;
// Bottom border
rect[1].x = x; rect[1].y = y + height - highlight_thickness;
rect[1].width = width; rect[1].height = highlight_thickness;
// Left border
rect[2].x = x; rect[2].y = y;
rect[2].width = highlight_thickness; rect[2].height = height;
// Right border
rect[3].x = x + width - highlight_thickness; rect[3].y = y;
rect[3].width = highlight_thickness; rect[3].height = height;

XFillRectangles(display, d, gc, rect, 4);
```

This approach is more efficient than drawing four separate rectangles or using multiple drawing calls.

### 5. Separator Drawing

The `XmeDrawSeparator` function creates clean visual dividers:

```c
void XmeDrawSeparator(Display *display, Drawable d,
                     GC top_gc, GC bottom_gc, GC separator_gc,
                     Position x, Position y,
                     Dimension width, Dimension height,
                     Dimension shadow_thick,
                     Dimension margin, unsigned char orientation,
                     unsigned char separator_type)
```

#### Separator Features

- **Orientation Support** - Both horizontal and vertical separators
- **Margin Handling** - Configurable margins around separators
- **Multiple Styles** - Different separator appearances
- **Shadow Integration** - Consistent with overall shadow system

## Performance Optimizations

### 1. XSegments vs XRectangles

The code explicitly mentions why XSegments are preferred:

```c
/* New implementation (1.2 vs 1.1) uses XSegments instead of XRectangles. */
/* Used for the simple shadow, the etched shadow and the separators */
/* Segment has been faster than Rectangles in all my benches, either
   on Hp, Sun or Pmax. Lines has been slower, that I don't understand... */
```

This optimization provides significant performance improvements across different hardware platforms.

### 2. Memory Reuse

The static allocation strategy in `DrawSimpleShadow` avoids repeated memory allocations:

```c
static XSegment * segms = NULL;
static int segm_count = 0;
```

This is particularly important for widgets that frequently redraw their shadows.

### 3. Batch Drawing Operations

The use of `XDrawSegments` and `XFillRectangles` allows the X server to optimize multiple drawing operations:

```c
XDrawSegments(display, d, top_gc, &segms[0], size2);
XDrawSegments(display, d, bottom_gc, &segms[size2], size2);
```

## Thread Safety Considerations

The drawing functions implement proper locking mechanisms:

```c
_XmProcessLock();
// ... drawing operations ...
_XmProcessUnlock();
```

And for application-level operations:

```c
_XmAppLock(app);
// ... drawing operations ...
_XmAppUnlock(app);
```

This ensures thread safety in multi-threaded Motif applications.

## Advanced Shadow Techniques

### 1. Corner Handling

The `cor` parameter in `DrawSimpleShadow` controls corner rendering:

```c
/*  Bottom segments  */
segms[i + size2].x1 = x + i + ((cor)?0:1);
/*  Right segments  */
segms[i + size3].y1 = y + i + 1 - cor;
```

This allows for different corner styles, from sharp to rounded appearances.

### 2. Shadow Thickness Limits

The automatic thickness calculation ensures visual consistency:

```c
ASSIGN_MIN(shadow_thick, (width >> 1));
ASSIGN_MIN(shadow_thick, (height >> 1));
if (shadow_thick <= 0) return;
```

This prevents shadows from overwhelming small widgets.

## Integration with Widget System

The drawing functions are designed to integrate seamlessly with the Motif widget system:

1. **Resource Integration** - Shadow properties are managed through the resource system
2. **Event Handling** - Drawing operations respect widget state changes
3. **Layout Management** - Shadow calculations consider widget dimensions and positions
4. **Theme Support** - Different visual themes can customize drawing behavior

## Best Practices for Using the Drawing System

### 1. GC Management

- Use appropriate GCs for different shadow types
- Ensure GCs have correct foreground colors for shadows
- Consider GC caching for frequently used drawing operations

### 2. Performance Considerations

- Batch drawing operations when possible
- Use appropriate shadow thickness for widget size
- Consider caching drawn elements for static content

### 3. Visual Consistency

- Maintain consistent shadow directions across the interface
- Use appropriate shadow types for different widget states
- Ensure shadows work well with the overall color scheme

## Conclusion

The Motif drawing system represents a masterclass in efficient, flexible graphics programming. By analyzing the `Draw.c` module, we can see how careful attention to performance, memory management, and visual consistency creates a robust foundation for the entire widget toolkit.

The sophisticated shadow algorithms, efficient memory usage, and comprehensive API design make this system both powerful and maintainable. Understanding these implementation details provides valuable insights for developers working with Motif or building similar graphics systems.

In the next post, we'll explore the widget implementation details and how these drawing functions integrate with the broader widget architecture.

---

*This blog post is part of a series exploring the Motif architecture. The drawing system analysis continues in our next post about widget implementation details.*
