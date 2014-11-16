#include "huffman.h"
#include <map>
#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <stack>
#include <stdio.h>

using namespace std;
        
const map<unsigned char, int> create_char_map(const char* inputfile)
{
    ifstream is (inputfile, ifstream::binary);
    map<unsigned char, int> char_map;
    
    char tmp;
    //cout << "Reading ";
    while(is.get(tmp))
    {
        unsigned char c;
        c=static_cast<unsigned char>(tmp);
        //printf("%x ", c);
        char_map[c]++;
    }
    //cout << "done." << endl;
    return char_map;
}

struct CompareNodes
{
    bool operator() (const Node* a, const Node* b) const{ return a->sum() > b->sum(); }
};

vector<bool> code;
map<unsigned char, vector<bool> > table;
void build_table(const Node* cur)
{
    if (cur->left()!=NULL && cur->right()!=NULL)
    {
        code.push_back(0);
        build_table(cur->left());
        code.push_back(1);
        build_table(cur->right());
    } else {
        table[cur->value()]=code;
        //printf("%x :", cur->value());
        //for(size_t i=0; i<code.size(); ++i) cout<<code[i];
        //cout<<endl;
    }

    code.pop_back();
}

const Node* create_tree(map<unsigned char, int> char_map)
{
    priority_queue<Node*, vector<Node*>, CompareNodes> temp_queue;
    for (map<unsigned char, int>::iterator itr=char_map.begin(); itr != char_map.end(); ++itr)
    {
        Node *p = new Node(itr->first, itr->second);
        temp_queue.push(p);
    }
    
    while(temp_queue.size()!=1) {
        Node *child_l = temp_queue.top();
        temp_queue.pop();
        Node *child_r = temp_queue.top();
        temp_queue.pop();

        Node *parent = new Node(child_l, child_r);
        temp_queue.push(parent);
    }

    Node *root = temp_queue.top();        
    return root;
}


void dfs(const Node* node, unsigned char &buff, int &cnt, ofstream &outfile)
{
    if (cnt == 8)
    {
        outfile.write((char*)&buff, sizeof(unsigned char));
        cnt = 0; buff = 0;
    }
    
    if (node->left()!=NULL && node->right()!=NULL)
    {
        //cout << "0 node go left" << endl;
        cnt++;
        dfs(node->left(), buff, cnt, outfile);
        //cout << "go right" << endl;
        dfs(node->right(), buff, cnt, outfile);
    } else {
        const unsigned char c = node->value();
        //printf("1 node, char: %x \n", c);    
        buff |= 0x01 << cnt++;
        buff |= c << cnt;
        //printf("\t %x \n", buff);
        outfile.write((char*)&buff, sizeof(unsigned char));
        buff = 0;
        buff |= c >> (8-cnt);
    }
}

void create_file(const char *outputfile, const char *inputfile, const Node* root)
{
    build_table(root); 

    ofstream outfile (outputfile, std::ofstream::binary);
    ifstream infile (inputfile, ifstream::binary);

    unsigned char buff=0;
    int cnt=0;
    //Кодирование дерева: записываются узлы в порядке обхода в глубину, если узел не лист, то 0 иначе 1. За 1 следуют 8 бит - символ из таблицы.
    
    outfile.write((char*)&buff, sizeof(unsigned char)); //Зарезервированное место под число свобоных(или занятых, как там дальше получится) бит в последнем байте
    
    dfs (root, buff, cnt, outfile);    
    //Кодирование по табличке
    unsigned char cur_sym;
    char tmp;
    while (infile.get(tmp))
    {
        cur_sym = static_cast<unsigned char>(tmp);
        //infile.read((char* )&cur_sym, sizeof(unsigned char));
        vector<bool> code = table[cur_sym];
        //printf("%x ",cur_sym); 
        for (size_t i = 0; i<code.size(); ++i)
        {
            if (++cnt == 8) { outfile.write((char*)&buff, sizeof(unsigned char)); cnt=0; buff=0; }
            buff |= code[i] << cnt;
        }
    }
    //cout<<endl<<cnt;
    outfile.write((char*)&buff, sizeof(unsigned char));
    
    buff = cnt;
    outfile.seekp (0, ios::beg);
    outfile.write((char*)&buff, sizeof(unsigned char));   

    infile.close();
    outfile.close();
}

/* ==============Декодер============= */
Node* read_tree(unsigned char &buff, int &cnt, ifstream &infile)
{
    if(cnt == 8) { cnt=0; infile.read((char*)&buff, sizeof(unsigned char)); }
    
    Node* cur = new Node();
    bool cur_bit = ((buff >> cnt) & 0x01) && 0x01;
    //cout << "Current bit is: " << cur_bit; 

    if (cur_bit) 
    {
        //Read chracter
        cnt++;
        
        unsigned char c=0;
        c |= buff >> cnt;
        infile.read((char* )&buff, sizeof(unsigned char));
        c |= buff << (8-cnt);
        cur->value_=c&0xff;
        
        //printf("\tWow, a character: %c (%x) \n", c, c & 0xff);
    } else {
        //cout << " next" << endl;
        cnt++;
        cur->left_ = read_tree(buff, cnt, infile);  
        cur->right_ = read_tree(buff, cnt, infile);
    }
    
    return cur;
}

void decrypt(const char *inputfile, const char *outputfile)
{
    ifstream infile(inputfile, ifstream::binary );
    ofstream outfile(outputfile, ofstream::binary ); 

    unsigned char buff=0; int cnt=0;

    infile.read((char* )&buff, sizeof(unsigned char));
    //int magic = buff;
    infile.read((char* )&buff, sizeof(unsigned char));

    Node *root = read_tree(buff, cnt, infile);
    Node *node = root;  

    //cout<<endl<<cnt<<endl;
    while(!infile.eof())
    {
        if (++cnt == 8)
        {
            cnt = 0;
            //buff = buff2;
            infile.read((char*)&buff, sizeof(unsigned char));    
            //printf("%x ",buff);
        }
        bool bit = ((buff >> cnt) & 0x01) && 0x01;
        
        if (bit) {
            node = node->right_;
        } else {
            node = node->left_;
        }
        if (node->right_ == NULL && node->left_ == NULL)
        { 
            //printf("%x %s %d %x\n", buff, infile.eof()?"true":"false", cnt, node->value());
            unsigned char value = node->value();
            outfile.write((char*)&value, sizeof(unsigned char));
            node = root;
        }
        
    } ; 

    outfile.close();
    infile.close();
}

void encrypt(const char* inputfile, const char* outputfile)
{
    create_file(outputfile, inputfile, create_tree(create_char_map(inputfile)));    
}
