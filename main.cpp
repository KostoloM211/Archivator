#include <iostream>
#include <queue>
#include <string>
#include <set>
#include <map>
#include <iomanip>

using namespace std;

struct TreeNode {
    char* c;
    int priority;
    TreeNode* left;
    TreeNode* right;
    TreeNode() {};
    TreeNode(char* c , int p, TreeNode* l, TreeNode* r) : c(c), priority(p), left(l), right(r) {};
};

class Comp  {
public:
    bool operator () (const TreeNode a, const TreeNode b) {
        return a.priority > b.priority;
    }
};

map<char, string> tableHoffman;
int step[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8096, 16192};

int GetBit (unsigned char c, int j);
int GetBitAtPosition(vector<unsigned char>& uncoding, int pos);
void buildTreeDecoding(vector<unsigned char>& decoding, TreeNode* root, string& buf);
void buildTreeUncoding(vector<unsigned char>& uncoding, TreeNode* root, int& pos);

int GetBit (unsigned char c, int j) {
    unsigned char mask = 1 << (7 - j);
    if (c & mask) {
        return 1;
    }
    else {
        return 0;
    }
}
int GetBitAtPosition(vector<unsigned char>& uncoding, int pos) {
    int sector = pos / 8;
    int part = pos % 8;
    if (sector > uncoding.size() - 1) {
        return -1;
    }
    unsigned char c = uncoding[sector];
    return GetBit(c, part);
}

void buildTreeDecoding(vector<unsigned char>& decoding, TreeNode* root, string& buf) {
    if (root->c != NULL) {
        buf += "1";
        unsigned char ch = *root->c;
        for (int i = 0; i < 8; i++) {
            int bit = GetBit(ch, i);
            buf += to_string(bit);
        }
        while (buf.size() >= 8) {
            unsigned char ch1 = 0;
            for (int j = 7; j >= 0; j--) {
                if (buf[j] == '1') {
                    ch1 = ch1 | (1 << (7 - (j % 8)));
                }
            }
            string bufForBuf;
            decoding.push_back(ch1);
            for (int j = 8; j < buf.size(); j++) {
                bufForBuf += buf[j];
            }
            buf = bufForBuf;
        }
    }
    else {
        buf += "0";
        while (buf.size() >= 8) {
            unsigned char ch = 0;
            for (int j = 7; j >= 0; j--) {
                if (buf[j] == '1') {
                    ch = ch | (1 << (7 - (j % 8)));
                }
            }
            string bufForBuf;
            for (int j = 8; j < buf.size(); j++) {
                bufForBuf += buf[j];
            }
            decoding.push_back(ch);
            buf = bufForBuf;
        }
        buildTreeDecoding(decoding, root->left, buf);
        buildTreeDecoding(decoding, root->right, buf);
    }
}

void buildTreeUncoding(vector<unsigned char>& uncoding, TreeNode* root, int& pos) {
    int bit = GetBitAtPosition(uncoding, pos);
    if (bit == -1) {
        cout << "Warning!\n";
        return;
    }
    if (bit == 0) {
        root->c = NULL;
        root->priority = 0;
        root->left = new TreeNode(NULL, 0, NULL, NULL);
        pos++;
        buildTreeUncoding(uncoding, root->left, pos);
        root->right = new TreeNode(NULL, 0, NULL, NULL);
        buildTreeUncoding(uncoding, root->right, pos);
    }
    else {
        string buf;
        for (int j = pos + 1; j < pos + 1 + 8; j++) {
            int b = GetBitAtPosition(uncoding, j);
            buf += to_string(b);
        }
        unsigned char ch = 0;
        for (int j = 7; j >= 0; j--) {
            if (buf[j] == '1') {
                ch = ch | (1 << (7 - (j % 8)));
            }
        }
        root->c = new char(ch);
        pos += 9;
    }
}

void buildTable (TreeNode* root, string s) {
    if (root == NULL) {
        return;
    }
    if (root->c != NULL) {
        tableHoffman[*root->c] = s;
        return;
    }
    buildTable(root->left, s + "0");
    buildTable(root->right, s + "1");
}

void HoffmanArchiving(string inputString, FILE* output) {
    int len = inputString.size();
    multiset<char> data;

    for (auto i = begin(inputString); i != end(inputString); i++) {
        data.insert(*i);
    }
    
    map<char, int> freq; //частотность символов
    priority_queue<TreeNode, vector<TreeNode>, Comp> q;
    
    //построение приоритетной очереди
    for (auto i = begin(data); i != end(data); i++) {
        freq[*i]++;
    }
    for (auto i = begin(freq); i != end(freq); i++) {
        TreeNode node;
        node.c = new char (i->first);
        node.priority = i->second;
        node.left = node.right = NULL;
        q.push(node);
    }
    
    //построение дерева
    TreeNode* root = new TreeNode();
    if (q.size() == 0) {
        cout << "Empty text\n";
        return;
    }
    
    if (q.size() == 1) {
        root->priority = q.top().priority;
        root->c = q.top().c;
        root->left = NULL;
        root->right = NULL;
    }
    else {
        while (q.size() != 1) {
            TreeNode* first = new TreeNode(q.top());
            q.pop();
            TreeNode* second = new TreeNode(q.top());
            q.pop();
            cout << "first: " << first->priority << endl;
            cout << "second: " << second->priority << endl;
            TreeNode* newNode = new TreeNode();
            newNode->c = NULL;
            newNode->priority = first->priority + second->priority;
            newNode->left = first;
            newNode->right = second;
            cout << "newNode: " << newNode->priority << endl << endl;
            q.push(*newNode);
            
        }
        root->c = q.top().c;
        root->priority = q.top().priority;
        root->left = q.top().left;
        root->right = q.top().right;
    }
    q.pop();
    
    //построение таблицы хофмана
    buildTable(root, string());
    for (auto i = begin(tableHoffman); i != end(tableHoffman); i++) {
        cout << i->first << " " << i->second << endl;
    }
    cout << endl;
    
    //кодирование дерева
    vector<unsigned char> treeDecoding;
    string buf;
    buildTreeDecoding(treeDecoding, root, buf);
    if (buf.size() != 0) {
        unsigned char ch = 0;
        for (int j = buf.size() - 1; j >= 0; j--) {
            if (buf[j] == '1') {
                ch = ch | (1 << (7 - (j % 8)));
            }
        }
        treeDecoding.push_back(ch);
    }
    
    //закодирование в unsigned char
    string result;
    vector<unsigned char> decoding;
    
    for (auto i = begin(inputString); i != end(inputString); i++) {
        string s = tableHoffman.find(*i)->second;
        result += s;
        if (result.size() >= 8) {
            unsigned char ch = 0;
            for (int j = 7; j >= 0; j--) {
                if (result[j] == '1') {
                    ch = ch | (1 << (7 - (j % 8)));
                }
            }
            decoding.push_back(ch);
            string buf;
            for (int j = 8; j < result.size(); j++) {
                buf += result[j];
            }
            result = buf;
        }
    }
    unsigned char ost = 8 - result.size();
    if (result.size() != 0) {
        unsigned char ch = 0;
        for (int j = result.size() - 1; j >= 0; j--) {
            if (result[j] == '1') {
                ch = ch | (1 << (7 - (j % 8)));
            }
        }
        decoding.push_back(ch);
    }
    
    //запись в файл
    fwrite(&ost, sizeof(unsigned char), 1, output);
    for (auto i = begin(treeDecoding); i != end(treeDecoding); i++) {
        unsigned char c = *i;
        fwrite(&c, sizeof(unsigned char), 1, output);
    }
    for (auto i = begin(decoding); i != end(decoding); i++) {
        unsigned char c = *i;
        fwrite(&c, sizeof(unsigned char), 1, output);
    }
    cout << "ost: " << (int)ost << endl;
    cout << "treeDecoding" << endl;
    for (auto i = begin(treeDecoding); i != end(treeDecoding); i++) {
        unsigned char c = *i;
        for (int j = 0; j < 8; j++) {
            cout << GetBit(c, j);
        }
    }
    cout << endl;

    cout << "decoding" << endl;
    for (auto i = begin(decoding); i != end(decoding); i++) {
        unsigned char c = *i;
        for (int j = 0; j < 8; j++) {
            cout << GetBit(c, j);
        }
    }
    cout << endl;
}

void HoffmanUnarchiving (vector<unsigned char>& input, FILE* output) {    
    TreeNode newRoot;
    newRoot.c = NULL;
    newRoot.left = NULL;
    newRoot.right = NULL;
    newRoot.priority = 0;
    int pos = 8;
    
    int ost = input[0];

    buildTreeUncoding(input, &newRoot, pos);
    
    while (pos % 8 != 0) {
        pos++;
    }
    TreeNode* it = &newRoot;

    for (int i = pos / 8; i < input.size(); i++) {
        unsigned char c = input[i];
        if (i == input.size() - 1) {
            for (int j = 0; j <= ost; j++) {
                int bit = GetBit(c, j);
                if (it->c != NULL) {
                    fwrite(it->c, sizeof(unsigned char), 1, output);
                    it = &newRoot;
                }
                if (bit == 1) {
                    it = it->right;
                }
                else {
                    it = it->left;
                }
            }
        }
        else {
            for (int j = 0; j < 8; j++) {
                int bit = GetBit(c, j);
                if (it->c != NULL) {
                    fwrite(it->c, sizeof(unsigned char), 1, output);
                    it = &newRoot;
                }
                if (bit == 1) {
                    it = it->right;
                }
                else {
                    it = it->left;
                }
            }
        }
    }
}


void RLEArchiving(string inputString, FILE* output) {
    vector<pair<char, int> > compressed;
    vector<unsigned char> decode;
    
    char prev = inputString[0];
    int cnt = 1;
    for (int i = 1; i < inputString.size(); i++) {
        if (inputString[i] == prev) {
            cnt++;
        }
        else {
            compressed.push_back(make_pair(prev, cnt));
            cnt = 1;
        }
        prev = inputString[i];
    }
    if (cnt != 1) {
        compressed.push_back(make_pair(prev, cnt));
    }
    else {
        compressed.push_back(make_pair(inputString[inputString.size()-1], cnt));
    }
    
    int count = 0;
    for (int i = 0; i < compressed.size(); i++) {
        
        if (compressed[i].second != 1) {
            if (count != 0) {
                decode.push_back(count - 1);
                for (int j = i - count; j < i; j++) {
                    decode.push_back(compressed[j].first);
                }
                count = 0;
            }
            unsigned char c;
            c = 1 << 7;
            c = c | (compressed[i].second - 2);
            decode.push_back(c);
            decode.push_back(compressed[i].first);
        }
        else {
            count++;
        }
//        cout << compressed[i].second << " " << compressed[i].first << endl;
    }
    
    if (compressed[compressed.size() - 1].second == 1) {
        if (count != 0) {
            unsigned char c = 0;
            c = c | (count - 1);
            decode.push_back(c);
            for (int j = compressed.size() - count; j < compressed.size(); j++) {
                decode.push_back(compressed[j].first);
            }
            count = 0;
        }
    }
    for (int i = 0; i < decode.size(); i++) {
        fwrite(&decode[i], sizeof(unsigned char), 1, output);
    }
}
void RLEUnarchiving (vector<unsigned char>& input, FILE* output) {
    string exitString;
    int i = 0;
    while (i < input.size()) {
        if (input[i] >= 128) {
            int cnt = input[i] - 128 + 2;
            for (int j = 0; j < cnt; j++) {
                exitString += input[i+1];
            }
            i += 2;
        }
        else {
            int cnt = input[i] + 1;
            for (int j = i + 1; j < i + 1 + cnt; j++) {
                exitString += input[j];
            }
            i += cnt + 1;
        }
    }
    for (int i = 0; i < exitString.size(); i++) {
        fwrite(&exitString[i], sizeof(char), 1, output);
    }
}

void LZWArchiving(string inputString, FILE* output) {
    
    map<string, int> table;
    int codebits = 7;
    int maximum = 128;
    cout << "minbit: " << codebits << endl;
    
    int cnt = 0;
    for (unsigned char i = 0; i < 128; i++) {
        string forInsert;
        forInsert += i;
        table.insert(make_pair(forInsert, cnt));
        cnt++;
    }

    string exitString;
    vector<unsigned char> answer;
    bool end = false;
    string buf;
    
    
    for (int i = 0; i < inputString.size(); i++) {
        string tmp = "";
        int k = 0;
        while (table.count(tmp + inputString[i + k]) == 1 && i + k != (inputString.size())) {
            tmp += inputString[i + k];
            k++;
        }
        cout << "tmp: " << tmp << endl;
//        if (i + k == (inputString.size())) {
//            exitString += table.find(tmp)->second;
//            if (exitString.size() >= 8) {
//                buf = "";
//                for (int j = 8; j < exitString.size(); j++) {
//                    buf += exitString[j];
//                }
//                unsigned char ch = 0;
//                for (int j = 7; j >= 0; j--) {
//                    if (exitString[j] == '1') {
//                        ch = ch | (1 << (7 - (j % 8)));
//                    }
//                }
//                answer.push_back(ch);
//                exitString = buf;
//            }
//            
//            end = true;
//        }
//        if (end) {
//            break;
//        }
//        else {
            string result;
            int number = table.find(tmp)->second;
            while (number) {
                result = to_string(number % 2) + result;
                number /= 2;
            }
            while (result.size() != codebits) {
                result = "0" + result;
            }
            cout << "result: " << result << endl;
            exitString += result;
            cout << "exitString: " << exitString << endl;
            while (exitString.size() >= 8) {
                buf = "";
                for (int j = 8; j < exitString.size(); j++) {
                    buf += exitString[j];
                }
                unsigned char ch = 0;
                for (int j = 7; j >= 0; j--) {
                    if (exitString[j] == '1') {
                        ch = ch | (1 << (7 - (j % 8)));
                    }
                }
                answer.push_back(ch);
                exitString = buf;
            }
            string res;
            int code = cnt;
            while (code) {
                res = to_string(code % 2) + res;
                code /= 2;
            }
            while (res.size() < codebits) {
                res = "0" + res;
            }
            cout << "insert: " << tmp + inputString[i + k] << " with cnt: " << cnt << " and res: " << res << endl;
            table.insert(make_pair(tmp + inputString[i + k], cnt));
            //displayTable(table);
            cnt++;
            if (cnt - 1 == maximum) {
                maximum *= 2;
                codebits++;
            }
            i += tmp.size() - 1;
//        }
    }
    cout << exitString << endl;
    if (exitString.size()) {
        unsigned char ch = 0;
        for (int j = exitString.size() - 1; j >= 0; j--) {
            if (exitString[j] == '1') {
                ch = ch | (1 << (7 - (j % 8)));
            }
        }
        answer.push_back(ch);
    }
    
    
    unsigned char ost = 8 - exitString.size();
    fwrite(&ost, sizeof(unsigned char), 1, output);
    
    for (int i = 0; i < answer.size(); i++) {
        fwrite(&answer[i], sizeof(unsigned char), 1, output);
    }
    
    string res;
    for (int j = 0; j < 8; j++) {
        int bit = GetBit(ost, j);
        res += to_string(bit);
    }
    res += " ";
    for (int i = 0; i < answer.size(); i++) {
        unsigned char cur = answer[i];
        for (int j = 0; j < 8; j++) {
            int bit = GetBit(cur, j);
            res += to_string(bit);
        }
        res += " ";
    }
    cout << res << endl;

}
void LZWUnarchiving (vector<unsigned char>& input, FILE* output) {
    int ost = input[0];
    int minbit = 7;
    int count = 128;
    map<int, string> newTable;
    

    int cnt = 0;
    for (unsigned char i = 0; i < 128; i++) {
        string forInsert;
        forInsert += i;
        newTable.insert(make_pair(cnt, forInsert));
        cnt++;
    }
    cout << endl;
    
    for (auto i = begin(newTable); i != end(newTable); i++) {
        cout << i->first << " " << i->second << endl;
    }
    
    string answer = "";
    string exitstring;
    
    string buf;
    int stPos = 8;
    cout << input.size() * 8 << endl;
    while (stPos < (input.size() * 8 - ost)) {
        int code = 0;
        int minstep = minbit;
        while (exitstring.size() != minbit) {
            int bit = GetBitAtPosition(input, stPos);
            if (bit) {
                code += step[minstep];
            }
            minstep--;
            exitstring += to_string(bit);
            stPos++;
        }
        cout << "exitString: " << exitstring << endl;
        buf += newTable.find(code)->second[0];
        cout << "buf: " << buf << endl;
        cout << "code: " << code << endl;
        if (buf.size() > 1) {
            cout << "insert: " << newTable.size() << " buf: " << buf << endl;
            newTable.insert(make_pair(newTable.size(), buf));
            buf = newTable.find(code)->second;

        }
 
        if (newTable.size() == count) {
            minbit++;
            count *= 2;
        }

        answer += newTable.find(code)->second;
        cout << "answer: " << answer << endl;
        exitstring = "";
    }
    cout << endl;
    fwrite(answer.c_str(), sizeof(unsigned char) * answer.size(), 1, output);
}


int main(int argc, const char * argv[]) {
    if (argc != 5) {
        cout << "ERROR: Input file name to archieve and method\n";
        return 0;
    }
    
    const char* fileName = argv[1]; //имя файла для архивации/разархивации
    const char* archieveName = argv[2]; //имя файла для архива/файла после разархивации
    const char* method = argv[3]; //используемый алгоритм архивации
    const char* arcOrNot = argv[4]; //архивировать/разархивировать
    FILE* input;
    FILE* output;
    
    string inputString;
    vector<unsigned char> forUnarchive;
    
    bool arc = false;
    if (strcmp(arcOrNot, "-arc") == 0) {
        arc = true;
    }
    else if (strcmp(arcOrNot, "-unarc") == 0) {
        arc = false;
    }
    else {
        cout << "ERROR: what to do: -arc or -unarc???\n";
    }
    
    if ((input = fopen(fileName, "r")) && arc == true) {
        char c;
        while ((c = fgetc(input)), c != EOF) {
            inputString += c;
        }
    }
    else if ((input = fopen(fileName, "r")) && arc == false) {
        unsigned char c;
        while (fread(&c, sizeof(unsigned char), 1, input) == 1) {
            forUnarchive.push_back(c);
        }
    }
    else {
        cout << "fopen failure\n";
        return 0;
    }

    if ((output = fopen(archieveName, "w"))) {
        if (strcmp(method, "-Hoffman") == 0 && arc == true) {
            HoffmanArchiving(inputString, output);
        }
        else if (strcmp(method, "-RLE") == 0 && arc == true) {
            RLEArchiving(inputString, output);
        }
        else if (strcmp(method, "-LZW") == 0 && arc == true) {
            LZWArchiving(inputString, output);
        }
        else if (strcmp(method, "-Hoffman") == 0 && arc == false) {
            HoffmanUnarchiving(forUnarchive, output);
        }
        else if (strcmp(method, "-RLE") == 0 && arc == false) {
            RLEUnarchiving(forUnarchive, output);
        }
        else if (strcmp(method, "-LZW") == 0 && arc == false) {
            LZWUnarchiving(forUnarchive, output);
        }
        else {
            cout << "Unnown method of archiving\n";
            return 0;
        }
    }
    else {
        cout << "fopen failure\n";
    }
    
    return 0;
}
