# XmString Test Suite Report
==================================================

## StringGLR

**Test Steps:**
1. Running StringGLR test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringDisp

**Test Steps:**
1. This is the StringDisp test to test display of XmString.

C)
2. The first two lines show hebrew strings.
3. The first line shows a LefttoRight string and the second line
   shows a RighttoLeft string.
4. The third and fourth lines shows kanji 16-bit character strings.
5. The third line shows a LefttoRight string, and the fourth line 
   shows a RighttoLeft string.

C)
6. Extensibility API test for XmeStringValid.  Tests if the function
   correctly determines a valid and invalid XmString.  Test results are
   dislplayed in the term window.

E)

**Automated Commands:**
- `Continue`
- `CompareVisual label1`
- `CompareVisual label2`
- `CompareVisual label3`
- `CompareVisual label4`
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringDraw

**Test Steps:**
1. This is the StringDraw test to test XmStringDraw.
2. You should see a BulletinBoard containing an empty DrawingArea 
   widget of size 300x200.

C)
3. There should be four lines showing the same string 
   "TEST STRING FOR CLIPPING", in font "9x15bold" (or "fixed" if 
   "9x15bold" is not available), with different levels of 
   clipping taking place.
4. Line 1 should show the string "TEST STRING FOR CLIPPING" complete.
5. Line 2 should show the string "TEST STRING FOR CLIPPING" with some
   horizontal clipping, meaning, some of the characters should be
   clipped off.
6. Line 3 should show the string "TEST STRING FOR CLIPPING" complete
   in the horizontal direction, but with characters getting clipped
   in height.
7. Line 4 should show the string "TEST STRING FOR CLIPPING" with
   both horizontal and vertical clipping.

E)

**Automated Commands:**
- `Continue`
- `CompareVisual drawArea`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringToByte

**Test Steps:**
1. Running StringToByte test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringToCT

**Description:**
XmSTRING_COMPONENT_CHARSET: ISO8859-1
@) XmSTRING_COMPONENT_DIRECTION: XmSTRING_DIRECTION_R_TO_L
@) XmSTRING_COMPONENT_TEXT: This is a test.
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_DIRECTION: XmSTRING_DIRECTION_L_TO_R
@) XmSTRING_COMPONENT_TEXT: This is only a test.

C)

**Test Steps:**
1. An XmString is created with direction, charset, text and separator
   components.  The function XmStringGetNextComponent() is then called
   to traverse the XmString and retrieve each component along with
   its values.  The component types and values should be printed
   to stdout.
2. Verify that the following values are printed:

@) XmSTRING_COMPONENT_CHARSET: ISO8859-1
@) XmSTRING_COMPONENT_DIRECTION: XmSTRING_DIRECTION_R_TO_L
@) XmSTRING_COMPONENT_TEXT: This is a test.
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_DIRECTION: XmSTRING_DIRECTION_L_TO_R
@) XmSTRING_COMPONENT_TEXT: This is only a test.

C)
3. The function XmCvtXmStringToCT() is called to convert the
   XmString to a (char *) string in compound text format.
   The converted string should be printed to stdout.
4. Verify that the following string is printed:

@) 2]This is a test.
@)
@) 1]This is only a test.
5. There should be two blank lines between the lines of text.

C)
6. The function XmCvtCTToXmString() is then called to convert the
   (char *) string in compound text format to an XmString.
   XmStringGetNextComponent() is then called to traverse the XmString
   and retrieve each of the components and their values.
   The components obtained and their values should be printed to stdout.
7. Verify that the following values are printed:

@) XmSTRING_COMPONENT_DIRECTION: XmSTRING_DIRECTION_R_TO_L
@) XmSTRING_COMPONENT_CHARSET: ISO8859-1
@) XmSTRING_COMPONENT_TEXT: This is a test.
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_SEPARATOR: True
@) XmSTRING_COMPONENT_DIRECTION: XmSTRING_DIRECTION_L_TO_R
@) XmSTRING_COMPONENT_CHARSET: ISO8859-1
@) XmSTRING_COMPONENT_TEXT: This is only a test.
8. The component types and component values are then compared
   to the component types and values in the original XmString.
   A message will be printed to stdout indicating whether
   XmCvtCTToXmString() passed or failed based on these
   comparison tests.

C)
E)

**Automated Commands:**
- `Continue`
- `Continue`
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringConcat

**Description:**
Running XmStringConcat.

**Test Steps:**
1. This test calls XmStringConcatAndFree() with every combination
   of two XmStringComponentTypes together.
   Messages will be printed to the terminal indicating the pair 
   of XmStringComponentTypes being passed to XmStringConcatAndFree()
   and whether the return of a new XmString was successful.
   Check that each combination passes. 

C)
E)

**Automated Commands:**
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## RendToProp

**Description:**
Running RendToProp.

**Test Steps:**
1. This test creates an XmRenderTable. The function XmRenderTableCvtToProp()
is then called to convert the XmRenderTable to a string representation.
A non-null string should be printed to stdout.
2. The function XmRenderTableCvtFromProp() is then called to convert
the string representation back into an XmRenderTable.  The converted
XmRenderTable is compared against the original XmRenderTable.
A message will be printed to stdout indicating whether the
function XmRenderTableCvtFromProp() passed or failed.

C)
E)

**Automated Commands:**
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringNext

**Description:**
This test checks that the XmString functions which parse a given
   XmString, return the components or segments of the XmString in
   the expected order along with the expected values.

**Test Steps:**
1. An XmString is created by concatenating a number of XmString components.
   The functions, XmStringPeekNextTriple() and XmStringGetNextTriple()
   are called to retrieve each of the components from the XmString.
   Messages should be printed to stdout listing the XmStringComponentType
   returned and whether the comparison test between the values expected and
   the actual values returned by the function passed or failed. 
   Verify that all the comparison tests passed.
   

C)
2. The same XmString is parsed again. This time, the functions
   XmStringPeekNextComponent() and XmStringGetNextComponent() are
   called to return the next component type. Messages should be printed
   to stdout listing the XmStringComponentType returned and whether
   the comparison test between the values expected and the actual
   values returned by the function passed or failed.  Verify that all the comparison
   tests passed.

C)
3. An XmString is created by concatenating a number of XmString segments.
   The function XmStringGetNextSegment() is then called to retrieve
   each of the segments.  Messages should be printed to stdout indicating
   whether the comparison between the segment information expected and
   the actual segment information returned by the function passed or failed.  Verify that
   all the comparison tests passed.

C)
4. An XmString is created with the following XmString components and their values,
   where appropriate, listed in parentheses.
!)
   XmSTRING_COMPONENT_LAYOUT_PUSH     ( XmLEFT_TO_RIGHT )
   XmSTRING_COMPONENT_RENDITION_BEGIN ( "render_tag1" )
   XmSTRING_COMPONENT_RENDITION_BEGIN ( "render_tag2" )
   XmSTRING_COMPONENT_TAG             ( XmSTRING_ISO8859-1 )
   XmSTRING_COMPONENT_TAB          
   XmSTRING_COMPONENT_DIRECTION       ( XmSTRING_DIRECTION_L_TO_R )
   XmSTRING_COMPONENT_TEXT            ( "Hello World" )
   XmSTRING_COMPONENT_RENDITION_END   ( "render_tag2" )
   XmSTRING_COMPONENT_RENDITION_END   ( "render_tag1" )
   XmSTRING_COMPONENT_LAYOUT_POP 
   XmSTRING_COMPONENT_SEPARATOR
5. The text "Hello World" should be rendered with the following attributes:
!) 
   Foreground Color:  red
   Font:              9x15
   Tab:               1 inch ( from the left margin )
6. The XmString component types will be printed to stdout in the order
   in which they have been retreived from the XmString.
   Verify that the component types returned conform to the following 
   order:

!)     [
        [ XmSTRING_COMPONENT_LAYOUT_PUSH ]
        [ XmSTRING_COMPONENT_RENDITION_BEGIN ]*
        [ XmSTRING_COMPONENT_TAG | XmSTRING_COMPONENT_LOCALE ]
        [ XmSTRING_COMPONENT_TAB ]*
        [ XmSTRING_COMPONENT_DIRECTION ]
        [ XmSTRING_COMPONENT_TEXT |
          XmSTRING_COMPONENT_LOCALE_TEXT |
          XmSTRING_COMPONENT_WIDECHAR_TEXT ]
        [ XmSTRING_COMPONENT_RENDITION_END ]*
        [ XmSTRING_COMPONENT_LAYOUT_POP ]
        [ XmSTRING_COMPONENT_SEPARATOR ] ]* XmSTRING_COMPONENT_END
C)
7. An XmString is created with the following XmString components and their values,
   where appropriate, listed in parentheses.
!)
   XmSTRING_COMPONENT_LAYOUT_PUSH     ( XmLEFT_TO_RIGHT )
   XmSTRING_COMPONENT_RENDITION_BEGIN ( "render_tag2" )
   XmSTRING_COMPONENT_RENDITION_BEGIN ( "render_tag1" )
   XmSTRING_COMPONENT_TAB
   XmSTRING_COMPONENT_DIRECTION       ( XmSTRING_DIRECTION_R_TO_L )
   XmSTRING_COMPONENT_LOCALE_TEXT     ( "Hello World" )
   XmSTRING_COMPONENT_DIRECTION       ( XmSTRING_DIRECTION_L_TO_R )
   XmSTRING_COMPONENT_RENDITION_END   ( "render_tag2" )
   XmSTRING_COMPONENT_LAYOUT_POP
   XmSTRING_COMPONENT_SEPARATOR
   XmSTRING_COMPONENT_RENDITION_END   ( "render_tag1" )
8. The text "dlroW olleH" should be rendered with the following attributes:

!) 
   Foreground Color:  black
   Font:              fixed
   Tab:               1.0 inch ( from the left margin )
9. The XmString component types will be printed to stdout in the order
   in which they have been retreived from the XmString.
   Verify that the component types returned conform to the following 
   order:

!)     [
         [ XmSTRING_COMPONENT_LAYOUT_PUSH ]
         [ XmSTRING_COMPONENT_RENDITION_BEGIN ]*
         [ XmSTRING_COMPONENT_TAG | XmSTRING_COMPONENT_LOCALE ]
         [ XmSTRING_COMPONENT_TAB ]*
         [ XmSTRING_COMPONENT_DIRECTION ]
         [ XmSTRING_COMPONENT_TEXT |
           XmSTRING_COMPONENT_LOCALE_TEXT |
           XmSTRING_COMPONENT_WIDECHAR_TEXT ]
         [ XmSTRING_COMPONENT_RENDITION_END ]*
         [ XmSTRING_COMPONENT_LAYOUT_POP ]
         [ XmSTRING_COMPONENT_SEPARATOR ] ]* XmSTRING_COMPONENT_END

C)
10. An XmString is created with the following XmString components and their values,
   where appropriate, listed in parentheses.
!)
   XmSTRING_COMPONENT_LAYOUT_PUSH     ( XmRIGHT_TO_LEFT )
   XmSTRING_COMPONENT_RENDITION_BEGIN ( "render_tag2" )
   XmSTRING_COMPONENT_DIRECTION       ( XmSTRING_DIRECTION_R_TO_L )
   XmSTRING_COMPONENT_WIDECHAR_TEXT   ( "Hi " )
   XmSTRING_COMPONENT_RENDITION_END   ( "render_tag2" )
   XmSTRING_COMPONENT_RENDITION_BEGIN ( "render_tag1" )
   XmSTRING_COMPONENT_TAB               
   XmSTRING_COMPONENT_DIRECTION       ( XmSTRING_DIRECTION_L_TO_R )
   XmSTRING_COMPONENT_TEXT            ( "World" )
   XmSTRING_COMPONENT_RENDITION_END   ( "render_tag1" )
   XmSTRING_COMPONENT_LAYOUT_POP
11. The text " World  iH" should be rendered with the following attributes:

!) 
"World"
   Foreground Color:  black
   Font:              fixed
   Tab:               1 inch ( from the right side of the word "iH" ) 

!) 
"iH"
   Foreground Color:  red
   Font:              9x15
   Tab:               None
12. The XmString component types will be printed to stdout in the order
   in which they have been retreived from the XmString.
   Verify that the component types returned conform to the following 
   order:

!)     [
         [ XmSTRING_COMPONENT_LAYOUT_PUSH ]
         [ XmSTRING_COMPONENT_RENDITION_BEGIN ]*
         [ XmSTRING_COMPONENT_TAG | XmSTRING_COMPONENT_LOCALE ]
         [ XmSTRING_COMPONENT_TAB ]*
         [ XmSTRING_COMPONENT_DIRECTION ]
         [ XmSTRING_COMPONENT_TEXT |
           XmSTRING_COMPONENT_LOCALE_TEXT |
           XmSTRING_COMPONENT_WIDECHAR_TEXT ]
         [ XmSTRING_COMPONENT_RENDITION_END ]*
         [ XmSTRING_COMPONENT_LAYOUT_POP ]
         [ XmSTRING_COMPONENT_SEPARATOR ] ]* XmSTRING_COMPONENT_END

C)
E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## FontListA

**Test Steps:**
1. Running FontListA test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringRend

**Description:**
This is the StringRend test used to test the display of
rendition components when drawing with XmString functions. 
Currently a blank drawing area widget should be displayed.

C) 

@) Eight strings should be displayed.  
The contents of the strings help to indicate which rendition 
components are currently active.

**Test Steps:**
1. The first string which reads \"Begin - Rendition 1\" should
be displayed with the following attributes:
@)  Background:  black
@)  Foreground:  white
@)  Font:        8x13
@)  Tabs:        None
@)  Underline:   No
@)  Strikethru:  No
2. The second string, \"Begin - Rendition 2\", should be displayed
with the attributes:
@)  Background:  blue
@)  Foreground:  white
@)  Font:        8x13
@)  Tabs:        None
@)  Underline:   Yes ( Single Line )
@)  Strikethru:  No

C)
3. The third string, \"Begin - Rendition 3\", should be displayed 
with the attributes:
@)  Background:  blue
@)  Foreground:  white
@)  Font:        8x13bold
@)  Tabs:        1 cm 
@)  Underline:   No
@)  Strikethru:  No
4. The fourth string, \"Begin - Rendition 4 - End\", should be displayed
with the attributes:
@)  Background:  blue
@)  Foreground:  yellow
@)  Font:        9x15
@)  Tabs:        2 cm 
@)  Underline:   No
@)  Strikethru:  Yes ( Single Dashed Line )

C)
5. The fifth string, \"Begin - Rendition 5 - End\", has all its resources
set to XmAS_IS. Its rendition resources should default to the resource
values of a previously active rendition which is Rendition 3. This string
should have the same display attributes as the third string.
with the attributes: 
@)  Background:  blue
@)  Foreground:  white
@)  Font:        8x13bold
@)  Tabs:        1 cm 
@)  Underline:   No
@)  Strikethru:  No
6. The sixth string, \"End - Rendition 2\", should be displayed with 
the following attributes: 
@)  Background:  black
@)  Foreground:  white
@)  Font:        8x13bold
@)  Tabs:        1 cm 
@)  Underline:   No
@)  Strikethru:  No

C)
7. The seventh string, \"Rendition 3 - End\", should have the same
display attributes as the previous string.
@)  Background:  black
@)  Foreground:  white
@)  Font:        8x13bold
@)  Tabs:        1 cm 
@)  Underline:   No
@)  Strikethru:  No
8. The eighth string, \"End - Rendition 1\", should be displayed with
the following attributes: 
@)  Background:  black
@)  Foreground:  white
@)  Font:        8x13
@)  Tabs:        None
@)  Underline:   No
@)  Strikethru:  No

C)

@)  A popup window titled \"Renditions\" should be displayed.
It contains a list of the current render tags within the render table
and three pushbuttons labelled \"Merge\", \"Modify\" and \"Remove\".
9. Select \"render2\" from the list in the \"Renditions\" window.
Select the \"Merge\" pushbutton. A popup window labelled \"Merge Panel\"
will be displayed.  The \"Merge Panel\" will list the current resource
values corresponding to the rendition selected.
10. Select \"NO_LINE" from the \"UnderlineType\" option menu.
Set the \"Merge Mode\" to \"Replace\". Select the \"Apply\" pushbutton. 
When \"Apply\" is selected, a new rendition is created using the same
render tag as the render tag selected in the \"Renditions\" window.
The new rendition is then added to the current render table with
the merge mode as XmREPLACE. Verify that there is no longer an underline
drawn beneath the second string in the \"Display Strings\" window.
11. Select the \"Quit\" pushbutton to popdown the \"Merge Panel\" window.

C)
12. Select \"render4\" from the \"Renditions\" window and then 
select the \"Merge\" pushbutton. The \"Merge Panel\" will be displayed
listing the current rendition resource values for the render tag selected. 
Select \"AS_IS\" from the \"StrikethruType\" option menu.
Select \"Merge New\" from the \"Merge Mode\" option menu.
Select the \"Apply\" pushbutton. Any unspecified resource values in the 
new rendition will be replaced with the values specified in the old
rendition. Verify that a single dashed line is still displayed through
the fourth string in the \"Display Strings\" window.
13. Select the \"Quit\" pushbutton to popdown the \"Merge Panel\" window.

C)
14. Select \"render5\" from the \"Renditions\" window and then select
the \"Merge\" pushbutton.  The \"Merge Panel\" should be displayed.
Type \"fixed\" into the \"FontName\" field and select \"FONT_IS_FONT\"
from the \"FontType\" option menu.  Select \"Merge Old\" from the \"Merge
Mode\" option menu and click the \"Apply\" pushbutton.
Since the merge mode is XmMERGE_OLD, any unspecified resource values from the
old rendition will be overwritten with a specified resource in the new
rendition.  Verify that the font is changed from 8x13bold to fixed in
the fifth string displayed in the \"Display Strings\" window.
15. Select the \"Quit\" pushbutton to popdown the \"Merge Panel\" window.

C)
16. Select \"render4\" from the \"Renditions\" window and then select
the \"Merge\" pushbutton.  The \"Merge Panel\" should be displayed.
Type \"0\" into the \"Foreground\" field and select \"NO_LINE\"
from the \"StrikethruType\" option menu.  Select \"Skip\" from the \"Merge
Mode\" option menu and click the \"Apply\" pushbutton.
Since the merge mode is XmSKIP, the new rendition resource values should
be ignored.  Verify that the foreground color remains yellow and the
strikethru line is still a single dashed line in the fourth string
displayed in the \"Display Strings\" window. 

C)
17. Select \"render1\" from the \"Renditions\" window and select the
\"Modify\" pushbutton. Change the value of the \"fontName\" field
to \"9x15\".  Change the value of the \"font\" field to \"Null\".
Click the \"Apply\" pushbutton.  The new fontName and font resource
values will be set and the function XmRenditionUpdate() will be called
to update the selected rendition.  Verify that the font for the first
second and eighth strings are changed to 9x15.

C)
18. Select \"render3\" from the \"Renditions\" window and select the
\"Remove\" pushbutton. The rendition, \"render3\", will be removed
from the render table and references to rendition components named
\"render3\" in the XmString should be ignored.
Verify that the third, fifth, sixth and seventh strings are no longer
tabbed and the font for the third, sixth and seventh strings
is the same as the font displayed in the first string.
 
C)
E)

**Automated Commands:**
- `Continue`
- `CompareVisual DrawArea`
- `Continue`
- `Continue`
- `Continue`
- `Continue`
- `LocatePointer RendList RListItem 1`
- `ClickMB Btn1`
- `LocatePointer MergePB`
- `ClickMB Btn1`
- `LocatePointer UnderlineOptions ROptionButton`
- `PressMB Btn1`
- `LocatePointer UnderlinePB1`
- `ReleaseMB Btn1`
- `LocatePointer ApplyPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea`
- `LocatePointer QuitPB`
- `ClickMB Btn1`
- `Continue`
- `LocatePointer RendList RListItem 3`
- `ClickMB Btn1`
- `LocatePointer MergePB`
- `ClickMB Btn1`
- `LocatePointer StrikethruOptions ROptionButton`
- `PressMB Btn1`
- `LocatePointer StrikethruPB0`
- `ReleaseMB Btn1`
- `LocatePointer MergeOptions ROptionButton`
- `PressMB Btn1`
- `LocatePointer MergeNewPB`
- `ReleaseMB Btn1`
- `LocatePointer ApplyPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea`
- `LocatePointer QuitPB`
- `ClickMB Btn1`
- `Continue`
- `LocatePointer RendList RListItem 4`
- `ClickMB Btn1`
- `LocatePointer MergePB`
- `ClickMB Btn1`
- `LocatePointer FontNameTF RTextChar Max`
- `ClickMB Btn1`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `InputString "fixed"`
- `LocatePointer FontTypeOptions ROptionButton`
- `PressMB Btn1`
- `LocatePointer FontTypePB1`
- `ReleaseMB Btn1`
- `LocatePointer MergeOptions ROptionButton`
- `PressMB Btn1`
- `LocatePointer MergeOldPB`
- `ReleaseMB Btn1`
- `LocatePointer ApplyPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea`
- `LocatePointer QuitPB`
- `ClickMB Btn1`
- `Continue`
- `LocatePointer RendList RListItem 3`
- `ClickMB Btn1`
- `LocatePointer MergePB`
- `ClickMB Btn1`
- `LocatePointer ForegroundTF RTextChar Max`
- `ClickMB Btn1`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `InputString "0"`
- `LocatePointer StrikethruOptions ROptionButton`
- `PressMB Btn1`
- `LocatePointer StrikethruPB1`
- `ReleaseMB Btn1`
- `LocatePointer MergeOptions ROptionButton`
- `PressMB Btn1`
- `LocatePointer SkipPB`
- `ReleaseMB Btn1`
- `LocatePointer ApplyPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea`
- `LocatePointer QuitPB`
- `ClickMB Btn1`
- `Continue`
- `LocatePointer RendList RListItem 0`
- `ClickMB Btn1`
- `LocatePointer ModifyPB`
- `ClickMB Btn1`
- `LocatePointer FontNameTF RTextChar Max`
- `ClickMB Btn1`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `InputString "9x15"`
- `LocatePointer FontTF RTextChar Max`
- `ClickMB Btn1`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `ClickKey KBackSpace`
- `InputString "Null"`
- `LocatePointer ApplyPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea`
- `LocatePointer QuitPB`
- `ClickMB Btn1`
- `Continue`
- `LocatePointer RendList RListItem 2`
- `ClickMB Btn1`
- `LocatePointer RemovePB`
- `ClickMB Btn1`
- `CompareVisual DrawArea`
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringComp

**Test Steps:**
1. Running StringComp test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringEmpty

**Test Steps:**
1. Running StringEmpty test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## FontListC

**Test Steps:**
1. Running FontListC test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringTab

**Description:**
The following test panel allows you to specify upto five tab settings
within the given string of text and then display the text with tabs inserted.

**Test Steps:**
1. Click BSelect at a position within the text widget where you would like
to insert a tab. Set values for the tab value, tab units and 
tab offset. Select the pushbutton labelled \"Set Tab\" to actually 
create the tab. A popup menu will appear displaying the current
tab values.
2. Repeat the above steps to insert more tabs into the text
string if desired.
3. Select pushbutton \"Draw String\" to display the string with
tabs included. The new text string will display below the text widget.
Verify that the tabs in the new text string are inserted at the correct 
positions and the tabs distances are correct.
4. Select \"Reset Tabs\" to remove the current tab settings.
5. Select \"Draw String\" to redisplay the text without any tabs.

C)

@) A table of XmStrings have been created with tabs and separators inserted.
When the strings are drawn, they should be displayed in three columns with
six rows of strings. The first row of strings contains the headings for each
column,  \"BREADS\", \"SALADS\", and \"SOUPS\". There should be three entries
in the \"BREADS\" column, four entries in the \"SALADS\" column and
five entries in the \"SOUPS\" column.
6. With the \"Tab Options\" set to \"Default Tabs\", click MB1 on the button
\"Draw Strings\". The strings will be displayed with a default tab 
of a half inch between each column of strings.

C)
7. Select the \"Proposed Tabs\" option from the \"Tab Options\" menu.
8. Specify the unit type used when creating tabs by selecting a type
from the \"Unit Model\" menu.
9. Specify the amount of padding used, based on the selected unit type,
to separate the columns of strings by setting \"Pad Value\".
10. Specify the offset model used in creating tabs by selecting an offset
from the \"Offset Model\" menu.
11. Click MB1 on the \"Draw Strings\" button.
12. Verify that the strings are displayed in three columns and six rows
with no overlapping between the rows and strings.

C)
E)

**Automated Commands:**
- `LocatePointer Text1 RTextChar 4`
- `ClickMB Btn1`
- `LocatePointer Scale1 RScaleSlider`
- `DragSliderValue Right 2`
- `LocatePointer UnitMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer UnitPushB6`
- `ReleaseMB Btn1`
- `LocatePointer OffsetMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer OffsetPushB0`
- `ReleaseMB Btn1`
- `LocatePointer CreateTabPB`
- `ClickMB Btn1`
- `LocatePointer Text1 RTextChar 8`
- `ClickMB Btn1`
- `LocatePointer Scale1 RScaleSlider`
- `DragSliderValue Left 1`
- `LocatePointer UnitMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer UnitPushB5`
- `ReleaseMB Btn1`
- `LocatePointer OffsetMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer OffsetPushB1`
- `ReleaseMB Btn1`
- `LocatePointer CreateTabPB`
- `ClickMB Btn1`
- `LocatePointer DrawStringPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea1`
- `LocatePointer ResetTabsPB`
- `ClickMB Btn1`
- `LocatePointer DrawStringPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea1`
- `Continue`
- `LocatePointer TabOptionsMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer TabOptionsPushB0`
- `ReleaseMB Btn1`
- `LocatePointer DrawStringPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea1`
- `Continue`
- `LocatePointer TabOptionsMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer TabOptionsPushB1`
- `ReleaseMB Btn1`
- `LocatePointer UnitMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer UnitPushB5`
- `ReleaseMB Btn1`
- `LocatePointer OffsetMenu ROptionButton`
- `PressMB Btn1`
- `LocatePointer OffsetPushB0`
- `ReleaseMB Btn1`
- `LocatePointer DrawStringPB`
- `ClickMB Btn1`
- `CompareVisual DrawArea1`
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringWidth

**Test Steps:**
1. Running StringWidth test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringExt

**Description:**
StringExt tests the XmString functions which get dimension information 
for displaying XmStrings. The values returned from the XmString functions
are compared against the values returned from the function XTextExtent().

**Test Steps:**
1. A blank drawing area widget should be displayed.
2. Click MB1 in the drawing area widget. The strings \"X is an Example of Z\"
and \"Z is an Example of X\" should now be displayed in the drawing area widget.
A rectangle is drawn around each string where the dimensions of the rectangle
are the dimensions returned from XmStringExtent(). Verify that the rectangles
represent the smallest bounding box which encloses each string.
3. Tables have been printed to stdout which list the functions called
and the dimensions returned from each function for the given string. 
A message will then be printed indicating whether each XmString function
passed or failed. If your term window is small, you may need to scroll
in order to see all the text. 

C)
E)

**Automated Commands:**
- `LocatePointer DrawingArea`
- `ClickMB Btn1`
- `CompareVisual DrawingArea`
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringCopy

**Test Steps:**
1. Running StringCopy test.

E)

**Automated Commands:**
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓

## StringParse

**Description:**
The following panels tests the XmParseMapping and XmStringParse API.
An empty drawing area widget should be displayed in the test window.

C)

**Test Steps:**
1. In this first panel, the character string \"One./Two/>Tab\" is parsed 
replacing the patterns \"/" with a separator and \">" with a tab.
2. XmStringParse() is called with XmNincludeStatus = XmTERMINATE for
each of the XmParseMapping entries of the XmParseTable.
The XmString should terminate after the first pattern is encountered.
Verify that the XmString displayed matches the text below:

@) One. 

C)
3. XmStringParse() is called with XmNincludeStatus = XmINSERT for
each of the XmParseMapping entries and the delimiter is a \".\".
The XmString should terminate prior to the \".\" character. 
Verify that the XmString displayed matches the text below:
 
@) One 

C)
4. XmStringParse() is called with XmNincludeStatus = XmINSERT for
each of the XmParseMapping entries and no delimiter is specified.
The XmString should terminate after the entire character string is parsed. 
Verify that the XmString displayed matches the text below, where the word
\"Tab\" is indented by one inch:

@) One.
@) Two
@) 	Tab

C)
5. Call XmStringUnparse() to retrieve the original character string
which was parsed to create an XmString.
A message will be printed to the terminal indicating whether or not
XmStringUnparse() was successful in generating the original character
string.

C)

@) A parse proc is assigned to the XmParseMapping which converts the
pattern \"/\" to a string.  Now when the pattern \"/\" is encountered,
the word \"String\" is inserted into the string prior to the separator.
6. When XmStringParse() is called, the XmString should terminate after
the first pattern is encountered.
Verify that the XmString displayed matches the text below:

@) One. String

C)
7. When XmStringParse() is called, the XmString should terminate at
the end of the character string. 
Verify that the XmString displayed matches the text below:

@) One. String
@) Two String
@) 	Tab

C)
8. A new parse table is created which replaces the pattern \">\"
with an XmString direction component with direction set to
XmSTRING_DIRECTION_L_TO_R and replaces the pattern \"<\" with
the an XmString direction component with direction set to
XmSTRING_DIRECTION_R_TO_L. 
XmStringTableParseStringArray() is called to parse an array 
of character strings containing these patterns and returns
a table of XmStrings.
The following strings should be displayed:

@) Left To Right
@) Right To Left
@) Left To Right
@) Right To Left
9. Call XmStringTableUnparse() to retrieve the original character string
which was parsed to create the XmStrings.
A message will be printed to the terminal indicating whether or not
XmStringTableUnparse() was successful in generating the original character
string.

C)
10. XmStringTableToXmString() is called to convert an array of XmStrings to
one XmString with a specified break component inserted between each
of the original XmStrings.
XmStringGetNextComponent() is then called to traverse the new XmString
to ensure each of the components is correct.
A message will be printed to the terminal indicating whether or not
XmStringTableToXmString() was successful in building the one XmString.
11. XmStringToXmStringTable() is then called to convert the one XmString
back to a table of XmStrings. Each XmString in the returned table is 
compared with its corresponding XmString in the original table.
If all the XmStrings match, the function was successful.
A message will be printed to the terminal indicating whether or not
XmStringToXmStringTable() was successful or not. 

C)
E)

**Automated Commands:**
- `Continue`
- `CompareVisual DrawArea`
- `Continue`
- `Continue`
- `CompareVisual DrawArea`
- `Continue`
- `CompareVisual DrawArea`
- `Continue`
- `Continue`
- `Continue`
- `Exit`

**File Status:**
- Executable: ✓
- Data file: ✓
- Script file: ✓
