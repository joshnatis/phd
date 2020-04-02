# phd
A simple Markdown to HTML parser, great for blog posts.

<img src="https://user-images.githubusercontent.com/31445542/77646031-27847580-6f3a-11ea-8b5c-d5868bf4b6db.png" alt="euripides" title="A cute euripides" width="20%">

*No need for Medical Doctors when you have a Doctor of Philosophy on hand :P*


## Install
* `git clone https://github.com/joshnatis/phd`
* edit the variables in the `config()` function on top of `phd.cpp` to configure your settings
* `make`
* Optional: install `highlight.js` from [their page](https://highlightjs.org/download/) for syntax highlighting in code blocks. If you choose not to install it, make sure to set the `USING_HIGHLIGHT_JS` flag to false in `phd.cpp`.

## Usage
* `./phd input.md output.html`
  
## Dependencies
a `c++11` compiler, a shell, `make` -- you have these things.

## Current features
* Headers
  * `#`, `##`, `###`, ... `######`
  * \<h1> through \<h6>
* Emphasis
  * `**` for **bold**, `~~` for ~~strikethrough~~, `__` for *italic*
  * Notice that the syntax for italics is different from traditional Markdown.
* Code blocks
  * ```` ```<optional language> <newline> ... <newline> ``` ````
  * automatic syntax highlighting if you link the `highlight.js` script
* Inline code
  * `` `This is inline code` ``
  * This surrounds the inline code in `<code>` tags and with the class `inline-code`. You can add styles for `.inline-code` to change how it looks.
* Images
  * `![alt](url)`
  * `![alt](url)|inline css, e.g. width: 50%; height: 30%|`
* Links
  * `[link text](https://url.com)`
  * I haven't implemented other types of links yet, and I don't plan to besides perhaps those in the following styles:  `[https://url.com]`, `<url.com>`, and `https://url.com`.
* \<hr> (horizontal rule)
    * `---`
* Line breaks
    * Blank lines mostly work the same as in Github-flavored Markdown. 
    * You can freely use `<br>` tags
* Blockquotes
    * `> this is all a blockquote`
    * Consecutive `>`'s with no blank lines between will all be funneled into a single blockquote
* Lists
    * `* `, `+ `, or `- ` for bulleted lists, `1.` (or any number) for numbered lists
    * To nest lists, include two spaces or a tab before your bullet point

## Missing features (for now)
* Inline HTML (mostly works, but I haven't done any tests yet)

## Missing features (forever)
* Everything else (*wait... there's not much else*)

## Note From the Author
I didn't use regex, so now I only have 99 problems instead of [2](https://blog.codinghorror.com/regular-expressions-now-you-have-two-problems/).

## Pictures!
I know you want em...

![Markdown](https://user-images.githubusercontent.com/31445542/77647237-6ddad400-6f3c-11ea-94b8-65900b429386.png)

---

![HTML](https://user-images.githubusercontent.com/31445542/77647239-6ddad400-6f3c-11ea-8dff-2325aea2b5e4.png)

