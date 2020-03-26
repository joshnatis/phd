#include <iostream>
#include <string>
#include <fstream>
#include <regex>

using namespace std;


void handle_errors(int argc, char **argv);
string parse(string md_line, ifstream &fin);
void parse_emphases_to_html(string &md_line);
void replace_emphases_helper(string &md_line, const string &md_symbol, 
	const string &open_html_tag, const string &close_html_tag);

//thank you https://stackoverflow.com/a/14678800
string replace(std::string subject, const std::string& search, 
	const std::string& replace);
//thank you https://stackoverflow.com/a/14678800
void replace2(std::string &subject, const std::string& search, 
	const std::string& replace);


int main(int argc, char **argv)
{
	handle_errors(argc, argv);

	string inputfn = argv[1];
	string outputfn = argv[2];

	ifstream fin(inputfn);
	if(fin.fail()) {std::cout << "Unable to read file.\n"; exit(1);}

	ofstream fout(outputfn);
	if(fout.fail()) {std::cout << "Unable to read file.\n"; exit(1);}

	string html_line;
	string md_line;
	while(getline(fin, md_line))
	{
		html_line = parse(md_line, fin);
		fout << html_line << endl;
		//cout << html_line << endl;
	}
}

bool contains_emphasis_symbols(const string &str)
{
	return str.find("_") != std::string::npos || 
		str.find("**") != std::string::npos ||
		str.find("~~") != std::string::npos;
}

string parse(string md_line, ifstream &fin)
{
	string html_result = "";

	//headers, except where entire line is made up of #s
	if(md_line[0] == '#' && replace(md_line, "#", "") != "")
	{
		int hash_count = 0;
		int i = 0;
		char c = md_line[0];

		while(c == '#' && hash_count < 6) //6 because of <h6>
		{
			++hash_count;
			++i;
			c = md_line[i];
		}

		//return right away cause headers don't need to be parsed further (no more styling)
		return "<h" + std::to_string(hash_count) + ">" + 
			md_line.substr(i) + "</h" + std::to_string(hash_count) + ">";
	}

	else if(md_line.substr(0,3) == "```")
	{
		string language = md_line.substr(3);
		html_result = "<pre><code class=\"" + language + "\">";
		string line;
		while(getline(fin, line))
		{
			if(line == "```")
			{
				html_result += "</code></pre>";
				return html_result;
			}
			else
			{
				replace2(line, "&", "&amp;");
				replace2(line, "<", "&lt;");
				replace2(line, ">", "&gt;");
				html_result += line + "\n";
			}
		}

		cout << "Invalid MD: Missing a closing code block! (```)\n";
		exit(1);
	}


	//bold, italics, and strikethrough
	else if(contains_emphasis_symbols(md_line))
	{
		parse_emphases_to_html(md_line);
	}

	if(md_line != "")
		html_result = "<p>" + md_line + "</p>";

	return html_result;
}

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
void replace2(std::string &subject, const std::string& search, const std::string& replace)
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



void parse_emphases_to_html(string &md_line)
{
	replace_emphases_helper(md_line, "**", "<b>", "</b>");
	replace_emphases_helper(md_line, "~~", "<s>", "</s>");
	replace_emphases_helper(md_line, "__", "<i>", "</i>");
}

void replace_emphases_helper(string &md_line, const string &md_symbol, 
	const string &open_html_tag, const string &close_html_tag)
{
	int open_index;
	int close_index;

	bool line_begins_with_symbol = md_line.substr(0, 2) == md_symbol;

	//while we find ** in the string
	while(line_begins_with_symbol || ((open_index = md_line.find(md_symbol)) && open_index != -1))
	{
		//there is a **

		//special case where ** are first 2 chars
		if(line_begins_with_symbol)
		{
			open_index = 0;
			//we dont want to enter the loop for this again
			line_begins_with_symbol = false;
		}

		//search for end in the rest of the string (rest = indeces after start + 2)
		close_index = md_line.find(md_symbol, open_index + 2);

		if(close_index != -1)
		{
			//there is a corresponding **

			//replace both ** with open and closed <b> tag
			md_line.replace(open_index, 2, open_html_tag);
			md_line.replace(close_index + 1, 2, close_html_tag);
		}
		else
		{
			//there's no end to the suffering (i.e. no closing **, so do nothing)
			break;
		}
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