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
int step[] = {};

void setSteps(int n) {
    int p = 2;
    step[0] = 0;
    step[1] = 1;
    for (int i = 2; i < n; i++) {
        step[i] = p;
        p *= 2;
    }
}

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

void buildTable (TreeNode* root, string s, bool& bugFix) {
    if (root == NULL) {
        return;
    }
    if (root->c != NULL && bugFix == true) {
        tableHoffman[*root->c] = "0";
        return;
    }
    bugFix = false;
    if (root->c != NULL) {
        tableHoffman[*root->c] = s;
        return;
    }
    buildTable(root->left, s + "0", bugFix);
    buildTable(root->right, s + "1", bugFix);
}

void HoffmanArchiving(string& inputString, FILE* output = NULL) {
    bool flag = false;
    if (output == NULL) {
        flag = true;
    }
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
//            cout << "first: " << first->priority << endl;
//            cout << "second: " << second->priority << endl;
            TreeNode* newNode = new TreeNode();
            newNode->c = NULL;
            newNode->priority = first->priority + second->priority;
            newNode->left = first;
            newNode->right = second;
//            cout << "newNode: " << newNode->priority << endl << endl;
            q.push(*newNode);
            
        }
        root->c = q.top().c;
        root->priority = q.top().priority;
        root->left = q.top().left;
        root->right = q.top().right;
    }
    q.pop();
    
    //построение таблицы хофмана
    bool bugFix = true;
    buildTable(root, string(), bugFix);
//    for (auto i = begin(tableHoffman); i != end(tableHoffman); i++) {
//        cout << i->first << " " << i->second << endl;
//    }
//    cout << endl;
//    
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
    if (flag) {
        inputString = "";
        inputString += ost;
        for (int i = 0; i < treeDecoding.size(); i++) {
            inputString += treeDecoding[i];
        }
        for (int i = 0; i < decoding.size(); i++) {
            inputString += decoding[i];
        }
    }
    else {
        fwrite(&ost, sizeof(unsigned char), 1, output);
        for (auto i = begin(treeDecoding); i != end(treeDecoding); i++) {
            unsigned char c = *i;
            fwrite(&c, sizeof(unsigned char), 1, output);
        }
        for (auto i = begin(decoding); i != end(decoding); i++) {
            unsigned char c = *i;
            fwrite(&c, sizeof(unsigned char), 1, output);
        }
    }

//    cout << "ost: " << (int)ost << endl;
//    cout << "treeDecoding" << endl;
//    for (auto i = begin(treeDecoding); i != end(treeDecoding); i++) {
//        unsigned char c = *i;
//        for (int j = 0; j < 8; j++) {
//            cout << GetBit(c, j);
//        }
//    }
//    cout << endl;

//    cout << "decoding" << endl;
//    for (auto i = begin(decoding); i != end(decoding); i++) {
//        unsigned char c = *i;
//        for (int j = 0; j < 8; j++) {
//            cout << GetBit(c, j);
//        }
//    }
//    cout << endl;
}

void HoffmanUnarchiving (vector<unsigned char>& input, FILE* output = NULL) {
    bool flag = false;
    vector<unsigned char> vector;
    
    if (output == NULL) {
        flag = true;
    }
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
                    if (flag) {
                        vector.push_back(*it->c);
                    }
                    else {
                        fwrite(it->c, sizeof(unsigned char), 1, output);
                    }
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
                    if (flag) {
                        vector.push_back(*it->c);
                    }
                    else {
                        fwrite(it->c, sizeof(unsigned char), 1, output);
                    }
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
    if (flag) {
        input = vector;
    }
}


void RLEArchiving(string& inputString, FILE* output = NULL) {
    bool flag = false;
    if (output == NULL) {
        flag = true;
    }
    vector<pair<char, int> > compressed;
    vector<unsigned char> decode;
    
    char prev = inputString[0];
    int cnt = 0;
    for (int i = 0; i < inputString.size(); i++) {
        if (inputString[i] == prev) {
            cnt++;
            if (cnt == 129) {
                compressed.push_back(make_pair(prev, 129));
                cnt = 0;
            }
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
        compressed.push_back(make_pair(inputString[inputString.size() - 1], cnt));
    }
    
    int count = 0;
    for (int i = 0; i < compressed.size(); i++) {
        if (compressed[i].second != 1) {
            if (count != 0) {
                do {
                    if (count > 128) {
                        decode.push_back(127);
                        for (int j = i - count; j < i - count + 128; j++) {
                            decode.push_back(compressed[j].first);
                        }
                        count -= 128;
                    }
                    else {
                        decode.push_back(count - 1);
                        for (int j = i - count; j < i; j++) {
                            decode.push_back(compressed[j].first);
                        }
                        count = 0;
                        break;
                    }
                }
                while (true);
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
    }
    
    if (compressed[compressed.size() - 1].second == 1) {
        do {
            if (count > 128) {
                decode.push_back(127);
                for (int j = compressed.size() - count; j < compressed.size() - count + 128; j++) {
                    decode.push_back(compressed[j].first);
                }
                count -= 128;
            }
            else {
                decode.push_back(count - 1);
                for (int j = compressed.size() - count; j < compressed.size(); j++) {
                    decode.push_back(compressed[j].first);
                }
                count = 0;
                break;
            }
        }
        while (true);
    }
    if (!flag) {
        for (int i = 0; i < decode.size(); i++) {
            fwrite(&decode[i], sizeof(unsigned char), 1, output);
        }
    }
    else {
        inputString = "";
        for (int i = 0; i < decode.size(); i++) {
            inputString += decode[i];
        }
    }

}
void RLEUnarchiving (vector<unsigned char>& input, FILE* output = NULL) {
    bool flag = false;
    if (output == NULL) {
        flag = true;
    }
    string exitString;
    vector<unsigned char> vector;
    int i = 0;
    while (i < input.size()) {
        if (input[i] >= 128) {
            int cnt = input[i] - 128 + 2;
            for (int j = 0; j < cnt; j++) {
                if (flag) {
                    vector.push_back(input[i + 1]);
                }
                else {
                    exitString += input[i + 1];
                }
            }
            i += 2;
        }
        else {
            int cnt = input[i] + 1;
            for (int j = i + 1; j < i + 1 + cnt; j++) {
                if (flag) {
                    vector.push_back(input[j]);
                }
                else {
                    exitString += input[j];
                }
            }
            i += cnt + 1;
        }
    }
    if (flag) {
        input = vector;
    }
    else {
        for (int i = 0; i < exitString.size(); i++) {
            fwrite(&exitString[i], sizeof(char), 1, output);
        }
    }
}

void LZWArchiving(string& inputString, FILE* output = NULL) {
    bool flag = false;
    if (output == NULL) {
        flag = true;
    }
    map<string, int> table;
    int codebits = 8;
    int maximum = 256;
    cout << "minbit: " << codebits << endl;
    
    int cnt = 0;
    for (int i = 0; i < 256; i++) {
        string forInsert;
        forInsert += (unsigned char)i;
        table.insert(make_pair(forInsert, cnt));
        cnt++;
    }

    string exitString;
    vector<unsigned char> answer;
    string buf;
    
    for (int i = 0; i < inputString.size(); i++) {
        string tmp = "";
        int k = 0;
        while (table.count(tmp + inputString[i + k]) == 1 && i + k != (inputString.size())) {
            tmp += inputString[i + k];
            k++;
        }
        string result;
        int number = table.find(tmp)->second;
        while (number) {
            result = to_string(number % 2) + result;
            number /= 2;
        }
        while (result.size() != codebits) {
            result = "0" + result;
        }
        exitString += result;
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
        if (i + k == inputString.size()) { //?
            break;
        }
//        cout << "insert: " << tmp + inputString[i + k] << " with cnt: " << cnt << " and res: " << res << endl;
        table.insert(make_pair(tmp + inputString[i + k], cnt));
        //displayTable(table);
        cnt++;
        if (cnt - 1 == maximum) {
            maximum *= 2;
            codebits++;
        }
        i += tmp.size() - 1;

    }
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
    
    if (flag) {
        inputString = "";
        inputString += ost;
        for (int i = 0; i < answer.size(); i++) {
            inputString += answer[i];
        }
    }
    else {
        fwrite(&ost, sizeof(unsigned char), 1, output);
        for (int i = 0; i < answer.size(); i++) {
            fwrite(&answer[i], sizeof(unsigned char), 1, output);
        }
    }
}
void LZWUnarchiving (vector<unsigned char>& input, FILE* output = NULL) {
    bool flag = false;
    vector<unsigned char> vector;
    if (output == NULL) {
        flag = true;
    }
    int ost = input[0];
    int minbit = 8;
    int count = 256;
    map<int, string> newTable;
    

    int cnt = 0;
    for (int i = 0; i < 256; i++) {
        string forInsert;
        forInsert += (unsigned char)i;
        newTable.insert(make_pair(cnt, forInsert));
        cnt++;
    }
    string answer = "";
    string exitstring;
    bool flag2 = false;
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
        buf += newTable.find(code)->second[0];
        if (flag2) {
            newTable.insert(make_pair(newTable.size(), buf));
            buf = newTable.find(code)->second;
        }
        flag2 = true;
 
        if (newTable.size() == count) {
            minbit++;
            count *= 2;
        }
        answer += newTable.find(code)->second;
        exitstring = "";
    }
    cout << endl;
    if (flag) {
        for (int i = 0; i < answer.size(); i++) {
            vector.push_back((unsigned char)answer[i]);
        }
        input = vector;
    }
    else {
        fwrite(answer.c_str(), sizeof(unsigned char) * answer.size(), 1, output);
    }
}



int main(int argc, const char * argv[]) {
    FILE* input = nullptr;
    string inputString;
    vector<unsigned char> forUnarchive;
    const char* keys = argv[1]; //ключи
    const char* archieveName = argv[2]; //имя файла
    
    //настройки по умолчанию
    bool arc = true;
    bool isArched = false;
    bool cKey = false;
    bool lKey = false;
    bool tKey = false;
    int lvlOfArching = 6;
    
    if (argc > 3) {
        cout << "USAGE: ./a.out [-123456789cdlt] [<file>]\n";
        return 0;
    }
    else if (argc == 2) {
        const char* file = argv[1];
        if ((input = fopen(file, "r"))) {
            keys = "-6";
            archieveName = file;
        }
        else {
            cout << "USAGE: ./a.out [-123456789cdlt] [<file>]\n";
            return 0;
        }
    }
    else if (argc == 1) {
        cout << "USAGE: ./a.out [-123456789cdlt] [<file>]\n";
        return 0;
    }
    else if (argc == 3) {
        keys = argv[1];
        archieveName = argv[2];
        if (!(input = fopen(archieveName, "r"))) {
            cout << "USAGE: ./a.out [-123456789cdlt] [<file>]\n";
            return 0;
        }
    }
    
    for (char* c = (char*)keys; *c != '\0'; c++) {
        if (*c == 'd') {
            arc = false;
        }
        else if (*c == 'c') {
            cKey = true;
        }
        else if (*c == 'l') {
            lKey = true;
        }
        else if (*c == 't') {
            tKey = true;
        }
        else if (*c > '0' && *c <= '9') {
            lvlOfArching = *c - '0';
        }
    }
    for (char* c = (char*)archieveName; *c != '\0'; c++) {
        if (*c == '.') {
            if (strcmp((const char*)(c + 1), "arch") == 0) {
                isArched = true;
                break;
            }
        }
    }
    if (arc && isArched) {
        cout << "file already arched\n";
        return 0;
    }
    if (!arc && !isArched) {
        cout << "file already unarched\n";
        return 0;
    }
    if (arc == true) {
        char c;
        while ((c = fgetc(input)), c != EOF) {
            inputString += c;
        }
        fclose(input);
        input = fopen(archieveName, "w");
        RLEArchiving(inputString);
        LZWArchiving(inputString);
        HoffmanArchiving(inputString, input);
    }
    else if (arc == false) {
        setSteps(31);
        unsigned char c;
        while (fread(&c, sizeof(unsigned char), 1, input) == 1) {
            forUnarchive.push_back(c);
        }
        fclose(input);
        input = fopen(archieveName, "w");
        HoffmanUnarchiving(forUnarchive);
        LZWUnarchiving(forUnarchive);
        RLEUnarchiving(forUnarchive, input);
    }


    return 0;
}
