#include <iostream>
#include <string>
#include <fstream>

using namespace std;

#define NOT_FOUND -1

string config(const string &inputfn)
{
	//============ CONFIG ============

	//==============================================================
	//* title of your HTML document/blog post
	//* (default: name of .md file)
	const string TITLE = inputfn.substr(0, inputfn.length() - 3);
	//==============================================================
	//* path to the folder containing your images
	//* (default: current folder)
	const string PATH_TO_IMAGE_DIR = ".";
	const string FAVICON = "favicon.ico";
	//==============================================================
	//* path to the folder containing your css
	//* (default: current folder)
	const string PATH_TO_STYLES_DIR = ".";
	const string USR_STYLESHEET = "styles.css";
	//==============================================================
	//* enable if using highlight.js for syntax highlighting
	const bool USING_HIGHLIGHT_JS = true;
	//* path to folder with higlight.js paraphernalia 
	const string PATH_TO_HIGHLIGHT_JS = "./highlight";
	//* one of the styles from highlight.js, in the styles/ dir
	const string SYNTAX_HIGHLIGHT_STYLE = "pojoaque.css";
	//==============================================================

	//============ \CONFIG ============

	string header = "<!DOCTYPE html>\n";
	header += "<html lang=\"en\">\n";
	header += "<head>\n";
	header += "\t<meta charset=\"utf-8\"/>\n";
	header += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n";
	header += "\t<title>" + TITLE + "</title>\n";
	header += "\t<link rel=\"stylesheet\" href=\"" + PATH_TO_STYLES_DIR + "/" + USR_STYLESHEET + "\">\n";
	header += "\t<link rel=\"icon\" href=\"" + PATH_TO_IMAGE_DIR + "/" + FAVICON + "\">\n";

	if(USING_HIGHLIGHT_JS)
	{
		header += "\t<link rel=\"stylesheet\" href=\"" + PATH_TO_HIGHLIGHT_JS + "/styles/" + SYNTAX_HIGHLIGHT_STYLE + "\">\n";
		header += "\t<script src=\"" + PATH_TO_HIGHLIGHT_JS + "/highlight.pack.js\"></script>\n";
		header += "\t<script>hljs.initHighlightingOnLoad();</script>\n";
	}

	header += "</head>\n\n<body>\n";

	return header;
}

void handle_errors(int argc, char **argv);
string parse(string md_line, ifstream &fin, int &consecutive_blank_lines, bool &unclosed_p_tag);
void parse_emphases_to_html(string &md_line);
void replace_emphases_helper(string &md_line, const string &md_symbol, 
	const string &open_html_tag, const string &closed_html_tag);
void parse_images_to_html(string &md_line);
void parse_urls_to_html(string &md_line);
void parse_inline_code_to_html(string &md_line);

//thank you https://stackoverflow.com/a/14678800
string replace(std::string subject, const std::string& search, 
	const std::string& replace);
//thank you https://stackoverflow.com/a/14678800
void replace_in_place(std::string &subject, const std::string& search, 
	const std::string& replace);


int main(int argc, char **argv)
{
	handle_errors(argc, argv);

	string inputfn = argv[1];
	string outputfn = argv[2];

	const string HEADER = config(inputfn);
	const string TAIL = "\n</body>\n</html>";

	ifstream fin(inputfn);
	if(fin.fail()) {std::cout << "Unable to read file.\n"; exit(1);}

	ofstream fout(outputfn);
	if(fout.fail()) {std::cout << "Unable to read file.\n"; exit(1);}

	fout << HEADER << endl;

	int consecutive_blank_lines = 0;
	bool unclosed_p_tag = false; //tracks state, if current line is inside unclosed <p>

	string html_line;
	string md_line;
	while(getline(fin, md_line))
	{
		html_line = parse(md_line, fin, consecutive_blank_lines, unclosed_p_tag);
		fout << html_line;

		if(consecutive_blank_lines >= 1 || !unclosed_p_tag)
			fout << endl;
	}

	if(unclosed_p_tag)
		fout << "\n</p>" << endl;

	fout << TAIL;
}

string parse(string md_line, ifstream &fin, int &consecutive_blank_lines, bool &unclosed_p_tag)
{
	string html_result = "";

	//headers, except where entire line is made up of #s
	if(md_line[0] == '#' && replace(md_line, "#", "") != "")
	{
		//headers can have inline code too!
		parse_inline_code_to_html(md_line);

		//prefer header tags to be outside of <p> tags
		if(unclosed_p_tag)
		{
			html_result = "</p>\n\n";
			unclosed_p_tag = false;
		}

		consecutive_blank_lines = 0;

		int hash_count = 0;
		int i = 0;
		char c = md_line[0];

		while(c == '#' && hash_count < 6) //6 because of <h6>
		{
			++hash_count;
			++i;
			c = md_line[i];
		}

		html_result += "<h" + to_string(hash_count) + ">" +
			md_line.substr(i) + "</h" + to_string(hash_count) + ">";

		//return right away cause headers don't need to be parsed further
		return html_result;
	}

	//horizontal rule
	else if(md_line == "---")
	{
		//<hr> tags can't be within <p> tags, so close any open <p>
		if(unclosed_p_tag)
		{
			html_result = "</p>\n\n";
			unclosed_p_tag = false;
		}

		consecutive_blank_lines = 0;
		return html_result + "<hr>";
	}

	//code blocks
	else if(md_line.substr(0,3) == "```")
	{
		//<pre> tags can't be within <p> tags, so close any open <p>
		if(unclosed_p_tag)
		{
			html_result += "</p>\n\n";
			unclosed_p_tag = false;
		}

		consecutive_blank_lines = 0;

		string language = md_line.substr(3);
		html_result += "<pre><code class=\"" + language + "\">";
		string line;
		while(getline(fin, line))
		{
			if(line == "```") //closing backticks
			{
				html_result += "</code></pre>";
				return html_result;
			}
			else
			{
				replace_in_place(line, "&", "&amp;");
				replace_in_place(line, "<", "&lt;");
				replace_in_place(line, ">", "&gt;");
				html_result += line + "\n";
			}
		}

		cout << "Invalid Markdown: Missing a closing code block! (```)\n";
		exit(1);
	}

	parse_inline_code_to_html(md_line);

	parse_images_to_html(md_line);
	//cause <img> doesn't automatically add a newline
	if(consecutive_blank_lines == 0 && md_line.find("<img") != NOT_FOUND)
		md_line = "<br>" + md_line;

	parse_urls_to_html(md_line);

	//bold, italics, and strikethrough
	parse_emphases_to_html(md_line);

	//consider making this a helper function
	if(md_line != "")
	{
		if(consecutive_blank_lines >= 1)
		{
			//there's been a blank md line, so the content should have a new line

			//this is achieved by either closing a <p> tag and starting a new one,
			if(unclosed_p_tag)
				html_result += "</p>\n\n<p>\n";

			//or by simply opening a <p> tag if one isn't already open
			else
			{
				html_result += "<p>\n";
				unclosed_p_tag = true;
			}

			consecutive_blank_lines = 0;
		}
		else
		{
			//continue appending content to the page as usual

			//"normal" text needs to be in some sort of tag, so place content in
			//<p> tag if one isn't already open
			if(!unclosed_p_tag)
			{
				html_result += "<p>\n";
				unclosed_p_tag = true;
			}
		}

		html_result += md_line;
	}
	else
		consecutive_blank_lines++;


	return html_result;
}

void parse_emphases_to_html(string &md_line)
{
	replace_emphases_helper(md_line, "**", "<b>", "</b>");
	replace_emphases_helper(md_line, "~~", "<s>", "</s>");
	replace_emphases_helper(md_line, "__", "<i>", "</i>");
}

//parses markdown emphasis syntax into html, in place
void replace_emphases_helper(string &md_line, const string &md_symbol, 
	const string &open_html_tag, const string &close_html_tag)
{
	const int symbol_len = 2; //this only works for symbols of length 2

	int open_index;
	int close_index;

	bool line_begins_with_symbol = md_line.substr(0, symbol_len) == md_symbol;

	//while we find ** in the string
	while(line_begins_with_symbol 
		|| ((open_index = md_line.find(md_symbol)) && open_index != NOT_FOUND))
	{
		//there is a **

		//special case where ** are first 2 chars
		if(line_begins_with_symbol)
		{
			open_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//search for end in rest of the string (rest = indeces after start + 2)
		close_index = md_line.find(md_symbol, open_index + symbol_len);

		if(close_index != NOT_FOUND)
		{
			//there is a corresponding **

			//replace both **'s with open and closed <b> tag
			md_line.replace(open_index, symbol_len, open_html_tag);
			md_line.replace(close_index + 1, symbol_len, close_html_tag);
		}
		else
		{
			//there's no end to the suffering (i.e no closing **, so do nothing)
			return;
		}
	}
}

//parses markdown image syntax into html, in place
void parse_images_to_html(string &md_line)
{
	//syntax: ![alt](url_to_image)
	//i've chosen to harcode strings and values as i think they're easier to read here

	bool line_begins_with_symbol = md_line.substr(0, 2) == "![";

	int open_bracket_index = 0;

	while(line_begins_with_symbol || 
		((open_bracket_index = md_line.find("![")) && open_bracket_index != NOT_FOUND))
	{
		//found an ![

		//special case where ![ are first 2 chars
		if(line_begins_with_symbol)
		{
			open_bracket_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//find first instance of ]( after [
		int closed_bracket_index = md_line.find("](", open_bracket_index);
		//find first instance of ) after ](
		int closed_paren_index = md_line.find(")", closed_bracket_index);

		if(closed_bracket_index != NOT_FOUND && closed_paren_index != NOT_FOUND)
		{
			//found ](

			//replace ]( with \" src=\" (hence the 2, we're replacing 2 chars, "[(")
			md_line.replace(closed_bracket_index, 2, "\" src=\"");
			//replace ![ with <img alt=\" (hence the 2, we're replacing 2 chars, "[(")
			md_line.replace(open_bracket_index, 2, "<img alt=\"");

			//weird math making up for extra chars we just inserted (html tags longer than md tags)
			//adjusted index = index - md length + html length
			const string open_alt_symbol = "<img alt=\"";
			closed_bracket_index = closed_bracket_index - 2 + open_alt_symbol.length();

			//find ) following the ](
			int closed_paren_index = md_line.find(")", closed_bracket_index);

			//replace ) with \"> (hence the 1, we're replacing 1 char)
			md_line.replace(closed_paren_index, 1, "\">");

			//FOR OPTIONAL INLINE CSS TAG
			int open_pipe_index = md_line.find("|", closed_paren_index);

			if(open_pipe_index != NOT_FOUND)
			{
				//found a |

				int closed_pipe_index = md_line.find("|", open_pipe_index + 1);
				if(closed_pipe_index != NOT_FOUND)
				{
					//found a closing |

					//get user specified styles
					string styles = md_line.substr(open_pipe_index + 1, 
						closed_pipe_index - open_pipe_index - 1);

					//remove styling metadata from content
					md_line.erase(open_pipe_index, closed_pipe_index - open_pipe_index + 1);

					//add styles to html
					int alt = md_line.find("alt", open_bracket_index);
					md_line.insert(alt, "style=\"" + styles + "\" ");
				}
			}
		}
		else return; //no bracket/paren, not a valid image
	}
}

//parses markdown url syntax into html, in place
void parse_urls_to_html(string &md_line)
{
	//syntax: [text](url)
	//i've chosen to harcode strings and values as i think they're easier to read here

	//most of the +1s and +2s are making up for index and length of symbols when getting text
	//e.g. to get [mytext, the index points to [, so we add 1 to the substring to get mytext

	bool line_begins_with_symbol = (md_line[0] == '[');

	int open_bracket_index = 0;

	while(line_begins_with_symbol || 
		((open_bracket_index = md_line.find("[")) && open_bracket_index != NOT_FOUND))
	{
		//found an [

		//special case where [ is first char
		if(line_begins_with_symbol)
		{
			open_bracket_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//find first instance of ]( after [
		int closed_bracket_index = md_line.find("](", open_bracket_index);
		int closed_paren_index = md_line.find(")", closed_bracket_index);

		if(closed_bracket_index != NOT_FOUND && closed_paren_index != NOT_FOUND)
		{
			//found ]( and )

			//hyperlink text (i.e. [the stuff in here](example.com))
			string text = md_line.substr(open_bracket_index + 1, 
				closed_bracket_index - open_bracket_index - 1);

			string url = md_line.substr(closed_bracket_index + 2, 
				closed_paren_index - closed_bracket_index - 2);

			//replace ]( with </a> (hence the 2, we're replacing 2 chars, "[(")
			md_line.replace(closed_bracket_index, 2, "\">");
			//replace [ with <a href=\" (hence the 1, we're replacing 1 char, "[")
			md_line.replace(open_bracket_index, 1, "<a href=\"");

			//weird math making up for extra chars we just inserted (html tags longer than md tags)
			//adjusted index = index - md length + html length
			const string open_a_symbol = "<a href=\"";
			closed_bracket_index = closed_bracket_index - 2 + open_a_symbol.length();

			//find ) following the ](
			closed_paren_index = md_line.find(")", closed_bracket_index);

			//replace ) with \"> (hence the 1, we're replacing 1 char)
			md_line.replace(closed_paren_index, 1, "</a>");

			//now swap text and url (cause the order is opposite in md and html)

			int text_pos = md_line.find(text, open_bracket_index);
			//replace text with random single char first,
			//cause if url length < text length, operation will leave behind part of text
			md_line.replace(text_pos, text.length(), "_");
			md_line.replace(text_pos, 1, url);

			//search for url after [stuff inside of brackets](
			//currently inside of brackets is url, so open brace + 1 + url length --> after
			int url_pos = md_line.find(url, open_bracket_index + 1 + url.length());
			md_line.replace(url_pos, url.length(), "_");
			md_line.replace(url_pos, 1, text);
		}
		else return; //no paren/bracket, it's not a valid url
	}
}

void parse_inline_code_to_html(string &md_line)
{
	//syntax: `this is all inline code`
	//i've chosen to harcode strings and values as i think they're easier to read here

	bool line_begins_with_symbol = (md_line[0] == '`');

	int open_backtick_index = 0;

	while(line_begins_with_symbol || 
		((open_backtick_index = md_line.find("`")) && open_backtick_index != NOT_FOUND))
	{
		//found a `

		//special case where ` is first char
		if(line_begins_with_symbol)
		{
			open_backtick_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//find first instance of ` after `
		int closed_backtick_index = md_line.find("`", open_backtick_index + 1);

		if(closed_backtick_index != NOT_FOUND)
		{
			//remove any special characters from code blocks
			for(int i = open_backtick_index + 1; i < closed_backtick_index; ++i)
			{
				if(md_line[i] == '<')
				{
					md_line.replace(i, 1, "&lt;");
					closed_backtick_index += 3; //difference in length of symbols
				}

				else if(md_line[i] == '>')
				{
					md_line.replace(i, 1, "&gt;");
					closed_backtick_index += 3; //difference in length of symbols
				}
			}

			md_line.replace(closed_backtick_index, 1, "</code>");
			md_line.replace(open_backtick_index, 1, "<code class=\"inline-code\">");
		}
		else return; //no closing backtick found
	}
}

/* 
==========
HELPERS
==========
*/

//thank you https://stackoverflow.com/a/14678800
string replace(std::string subject, const std::string& search, const std::string& replace)
{
	if(search == "")
		return "";

	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	
	return subject;
}

//thank you https://stackoverflow.com/a/14678800
void replace_in_place(std::string &subject, const std::string& search, const std::string& replace)
{
	if(search == "")
		return;

	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

void handle_errors(int argc, char **argv)
{
	if(argc != 3)
	{
		cout << "Usage: " << argv[0] << " input.md output.html\n";
		exit(1);
	}
	int l;

	string inputfn = argv[1];
	if((l = inputfn.length()) && (l < 3 || inputfn.substr(l - 3) != ".md"))
	{
		std::cout << "Invalid input file type (must end in .md)\n";
		exit(1);
	}

	string outputfn = argv[2];
	if((l = outputfn.length()) && (l < 5 || outputfn.substr(l - 5) != ".html"))
	{
		std::cout << "Invalid output file type (must end in .html)\n";
		exit(1);
	}
}