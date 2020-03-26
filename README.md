# phd
A half-baked Markdown to HTML parser, great for blog posts.

*No need for Medical Doctors when you have a Doctor of Philosophy on hand :P*

## Install
* `git clone https://github.com/joshnatis/phd`
* `make`
* `vim inject`
  * configure some of the variables in the script

## Usage
* `./a.out input.md output.html`
* `./inject`
  * inserts header and closing tags into html file
  
## Dependencies
Nothing (well, I guess a c++11 compiler and a shell), but you should download [highlight.js](https://highlightjs.org/) so you can have syntax highlighting for your code blocks. If you choose not to, make sure to edit the `inject` script to remove any references to it.

## Current features
* headers (h1 - h6)
* emphasis (`**` for **bold**, `~~` for ~~strikethrough~~, `__` for *italic*)
  * Notice that the syntax for italics is different than traditional Markdown.
* code blocks (automatic syntax highlighting if you link the `highlight.js` script)

## Missing features (for now)
* Lists
* Images
* Links
* Inline code
* Blockquotes
* \<hr> (horizontal rule)
* \<br> (line breaks)
* Inline HTML

## Missing features (forever)
* Everything else (*wait... there's not much else*)

## Syntax Highlighting
Aaaa check out [highlight.js](https://highlightjs.org/)

## Note From the Author
I didn't use regex, so now I only have 99 problems instead of [2](https://blog.codinghorror.com/regular-expressions-now-you-have-two-problems/).
