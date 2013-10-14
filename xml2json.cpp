#include <cstdio>
#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <json/json.h>
#include <xmlParser.h>

using namespace std;
int type = 0;
int mode = 0;

bool ParseXmlLevel(XMLNode &node, Json::Value &oValue);
void ParseFromFile(char *file);
void ParseFromStdin();
void help();

int main(int argc, char **argv)
{
    char file[256] = {0};
    int ch;
    while((ch = getopt(argc, argv, "f:c")) != -1) {
        switch(ch) {
            case 'f':
                strncpy(file, optarg, 256);
                mode = 1;
                file[255] = 0;
                break;
            case 'c':
                type = 1;
                break;
            default:
                help();
                return 1;
        }
    }
    // mode=1 从文件读取，0从终端读取
    if(mode == 1) {
        ParseFromFile(file);
    } else {
        ParseFromStdin();
    }
    return 0;
}

bool ParseXmlLevel(XMLNode &node, Json::Value &oValue)
{
    const char *ptr = node.getName();
    if(ptr == NULL) {
        cerr << "xml format error" << endl;
        return false;
    }
    string name(ptr);
    int attr_num = node.nAttribute();
    int node_num = node.nChildNode();
    if(node.isDeclaration() == 0) {
        for(int i = 0; i < attr_num; ++i) {
            XMLAttribute attr = node.getAttribute(i);
            oValue[attr.lpszName] = attr.lpszValue;
        }
    }

    if(node_num == 0) {
        int text_num = node.nText();
        if(text_num == 1) {
            oValue[name] = node.getText();
        } else {
            for(int i = 0; i < text_num; ++i) {
                oValue[name].append(node.getText(i));
            }
        }
        int clear_num = node.nClear();
        if(clear_num == 1) {
            XMLClear xmlClear = node.getClear();
            if((strncmp(xmlClear.lpszOpenTag,"<![CDATA[",9)==0) && (strncmp(xmlClear.lpszCloseTag, "]]>",9)==0)) {
                oValue[name] = xmlClear.lpszValue;
            }
        }
        return true;
    }
    for(int i = 0; i < node_num; ++i) {
        XMLNode cnode = node.getChildNode(i);
        string cname = cnode.getName();

        if(cnode.nChildNode() == 0) {
            int text_num = cnode.nText();
            if(text_num == 1) {
                oValue[cname] = cnode.getText();
            } else {
                for(int i = 0; i < text_num; ++i) {
                    oValue[cname].append(node.getText(i));
                }
            }
            int clear_num = cnode.nClear();
            if(clear_num == 1) {
                XMLClear xmlClear = cnode.getClear();
                if((strncmp(xmlClear.lpszOpenTag,"<![CDATA[",9)==0) && (strncmp(xmlClear.lpszCloseTag, "]]>",9)==0)) {
                    oValue[cname] = xmlClear.lpszValue;
                }
            } else {
                //暂时忽略
            }
        } else {
            Json::Value temp;
            ParseXmlLevel(cnode, temp);
            if(node.nChildNode(cname.c_str()) == 1) {
                oValue[cname] = temp;
            } else {
                oValue[cname].append(temp);
            }
        }
    }
    return true;
}

void ParseFromFile(char *file)
{
    Json::Value oValue;
    XMLNode root = XMLNode::parseFile(file);
    if(root.isEmpty()) {
        cerr << "file empty" << endl;
        return;
    }

    if(ParseXmlLevel(root, oValue)) {
        if(type == 1) {
            cout << oValue.toJsonString() << endl;
        } else {
            cout << oValue.toStyledString() << endl;
        }
    }

    return;
}

void ParseFromStdin()
{
    char buf[8193] = {0};
    int iRet = 0;
    int hasRead = 0;
    while((iRet = getchar())) {
        if((iRet == EOF)||(iRet == '\r')) {
            break;
        } else if(iRet < 0) {
            cerr << "read data from stdin error" << endl;
            break;
        }
        buf[hasRead++] = iRet;
    }
    // printf("%s\n",buf);

    Json::Value oValue;
    XMLNode root = XMLNode::parseString(buf);
    if(root.isEmpty()) {
        cerr << "file empty" << endl;
        return;
    }

    if(ParseXmlLevel(root, oValue)) {
        if(type == 1) {
            cout << oValue.toJsonString() << endl;
        } else {
            cout << oValue.toStyledString() << endl;
        }
    }
}

void help()
{
    cout << "-f [file]" << endl;
}
