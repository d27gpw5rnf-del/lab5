#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_set>
#include <random>
#include <iomanip>

using namespace std;

// ---------- Упорядоченный односвязный список ----------
struct ListNode {
    int key;
    ListNode* next;
    ListNode(int k) : key(k), next(nullptr) {}
};

class OrderedSinglyList {
private:
    ListNode* head;
    size_t nodeCount;
    size_t memoryUsed;

public:
    OrderedSinglyList() : head(nullptr), nodeCount(0), memoryUsed(0) {}

    void insert(int key) {
        ListNode* newNode = new ListNode(key);
        memoryUsed += sizeof(ListNode);

        if (!head || key < head->key) {
            newNode->next = head;
            head = newNode;
            nodeCount++;
            return;
        }

        ListNode* cur = head;
        while (cur->next && cur->next->key < key) {
            cur = cur->next;
        }

        // проверка дубликата
        if (cur->key == key) {
            delete newNode;
            memoryUsed -= sizeof(ListNode);
            return;
        }
        if (cur->next && cur->next->key == key) {
            delete newNode;
            memoryUsed -= sizeof(ListNode);
            return;
        }

        newNode->next = cur->next;
        cur->next = newNode;
        nodeCount++;
    }

    bool search(int key, int& comparisons) {
        comparisons = 0;
        ListNode* cur = head;
        while (cur) {
            comparisons++;
            if (cur->key == key) return true;
            if (cur->key > key) break;
            cur = cur->next;
        }
        return false;
    }

    size_t getMemory() const { return memoryUsed; }
    size_t size() const { return nodeCount; }

    ~OrderedSinglyList() {
        ListNode* cur = head;
        while (cur) {
            ListNode* next = cur->next;
            delete cur;
            cur = next;
        }
    }
};

// ---------- Рандомизированное бинарное дерево поиска (Treap) ----------
struct TreapNode {
    int key;
    int priority;
    TreapNode* left;
    TreapNode* right;
    TreapNode(int k) : key(k), priority(rand()), left(nullptr), right(nullptr) {}
};

class Treap {
private:
    TreapNode* root;
    size_t nodeCount;
    size_t memoryUsed;

    void rotateLeft(TreapNode*& node) {
        TreapNode* right = node->right;
        node->right = right->left;
        right->left = node;
        node = right;
    }

    void rotateRight(TreapNode*& node) {
        TreapNode* left = node->left;
        node->left = left->right;
        left->right = node;
        node = left;
    }

    void insert(TreapNode*& node, int key) {
        if (!node) {
            node = new TreapNode(key);
            memoryUsed += sizeof(TreapNode);
            nodeCount++;
            return;
        }
        if (key == node->key) return; // дубликаты игнорируются

        if (key < node->key) {
            insert(node->left, key);
            if (node->left->priority > node->priority)
                rotateRight(node);
        } else {
            insert(node->right, key);
            if (node->right->priority > node->priority)
                rotateLeft(node);
        }
    }

    bool search(TreapNode* node, int key, int& comparisons) {
        if (!node) return false;
        comparisons++;
        if (key == node->key) return true;
        if (key < node->key)
            return search(node->left, key, comparisons);
        else
            return search(node->right, key, comparisons);
    }

    void clear(TreapNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    Treap() : root(nullptr), nodeCount(0), memoryUsed(0) {
        srand(time(nullptr));
    }

    void insert(int key) { insert(root, key); }

    bool search(int key, int& comparisons) {
        comparisons = 0;
        return search(root, key, comparisons);
    }

    size_t getMemory() const { return memoryUsed; }
    size_t size() const { return nodeCount; }

    ~Treap() { clear(root); }
};

// ---------- Генерация файла с уникальными числами ----------
void generateDataFile(const string& filename, long long minVal, long long maxVal, int count) {
    random_device rd;
    mt19937_64 gen(rd());
    uniform_int_distribution<long long> dist(minVal, maxVal);
    unordered_set<long long> uniqueKeys;

    while (uniqueKeys.size() < count) {
        uniqueKeys.insert(dist(gen));
    }

    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Не удалось создать файл " << filename << endl;
        exit(1);
    }
    for (long long key : uniqueKeys) {
        fout << key << "\n";
    }
    fout.close();
    cout << "Сгенерирован файл " << filename << " (" << count << " уникальных чисел)\n";
}

// ---------- Чтение ключей из файла ----------
vector<int> readKeysFromFile(const string& filename) {
    vector<int> keys;
    ifstream fin(filename);
    if (!fin.is_open()) return keys;
    int x;
    while (fin >> x) keys.push_back(x);
    fin.close();
    return keys;
}

// ---------- Формирование тестового набора из 100 ключей ----------
vector<int> buildTestSet(const vector<int>& data) {
    vector<int> test;
    test.reserve(100);

    // присутствующие ключи (38 штук)
    if (!data.empty()) {
        size_t step = data.size() / 38;
        for (int i = 0; i < 38 && i * step < data.size(); ++i) {
            test.push_back(data[i * step]);
        }
    } else {
        for (int i = 0; i < 38; ++i) test.push_back(1000 + i * 1000);
    }

    int minKey = data.empty() ? 140 : *min_element(data.begin(), data.end());
    int maxKey = data.empty() ? 10000000 : *max_element(data.begin(), data.end());

    // меньшие минимального (13)
    for (int v : {1, 5, 10, 30, 50, 70, 90, 110, 120, 130, minKey - 3, minKey - 2, minKey - 1}) {
        if (v < minKey) test.push_back(v);
        else if (v == minKey) test.push_back(minKey - 1);
    }

    // большие максимального (13)
    for (int v : {maxKey + 1, maxKey + 10, maxKey + 50, maxKey + 100, maxKey + 500,
                  maxKey + 1000, maxKey + 50000, maxKey + 10000, maxKey + 500000,
                  maxKey + 1000000, maxKey + 5000000}) {
        test.push_back(v);
    }

    // промежуточные отсутствующие (14)
    int mid1 = minKey + (maxKey - minKey) / 7;
    int mid2 = minKey + (maxKey - minKey) / 4;
    int mid3 = minKey + (maxKey - minKey) / 2;
    int mid4 = minKey + (maxKey - minKey) * 3 / 4;
    int mid5 = minKey + (maxKey - minKey) * 5 / 6;
    for (int v : {mid1, mid1 + 1, mid2, mid2 + 2, mid3, mid3 + 5, mid4, mid4 + 7,
                  mid5, mid5 + 10, (minKey + maxKey) / 3, (minKey + maxKey) / 2 + 100,
                  maxKey / 2, maxKey / 3}) {
        test.push_back(v);
    }

    // близкие к существующим (10)
    if (!data.empty()) {
        for (size_t i = 0; i < 10 && i < data.size(); ++i) {
            test.push_back(data[i] + 1);
        }
    }

    // обрезаем до 100
    if (test.size() > 100) test.resize(100);
    while (test.size() < 100) test.push_back(99999999);

    return test;
}

// ---------- Главная функция ----------
#ifdef __linux__
    #error "Linux is not supported in this branch"
#endif
int main() {
    #ifdef _WIN32
    #error "Windows is not supported in this branch"
#endif
    setlocale(LC_ALL, "Russian");
    cout << "=== Вариант 24: Упорядоченный список vs Treap ===\n";

    const string filename = "test_numbers.txt";
    const long long MIN_VAL = 140;
    const long long MAX_VAL = 100000000;
    const int DATA_SIZE = 100000;

    vector<int> data = readKeysFromFile(filename);
    if (data.empty()) {
        cout << "Файл " << filename << " не найден или пуст. Генерируем данные...\n";
        generateDataFile(filename, MIN_VAL, MAX_VAL, DATA_SIZE);
        data = readKeysFromFile(filename);
    }
    if (data.empty()) {
        cerr << "Не удалось загрузить или сгенерировать данные. Программа завершена.\n";
        return 1;
    }

    cout << "Загружено элементов: " << data.size() << endl;

    OrderedSinglyList list;
    Treap treap;

    for (int key : data) {
        list.insert(key);
        treap.insert(key);
    }

    cout << "\nПамять, занятая структурами:\n";
    cout << "Список: " << list.getMemory() << " байт (размер узла: " << sizeof(ListNode) << ")\n";
    cout << "Treap: " << treap.getMemory() << " байт (размер узла: " << sizeof(TreapNode) << ")\n";

    vector<int> testKeys = buildTestSet(data);
    cout << "\nСформирован тестовый набор из " << testKeys.size() << " ключей.\n";

    long long totalCompList = 0, totalCompTreap = 0;
    int foundList = 0, foundTreap = 0;

    cout << "\nРезультаты поиска (первые 20 строк для краткости):\n";
    cout << left << setw(12) << "Ключ" << setw(20) << "Список (сравн.)" << "Treap (сравн.)" << endl;

    for (size_t i = 0; i < testKeys.size(); ++i) {
        int key = testKeys[i];
        int compList = 0, compTreap = 0;
        bool resList = list.search(key, compList);
        bool resTreap = treap.search(key, compTreap);
        totalCompList += compList;
        totalCompTreap += compTreap;
        if (resList) foundList++;
        if (resTreap) foundTreap++;

        if (i < 20) {
            cout << setw(12) << key << setw(20) << compList << compTreap << endl;
        }
    }

    cout << "\n=== Итоговые средние значения ===\n";
    cout << "Среднее число сравнений в списке: " << (double)totalCompList / testKeys.size() << endl;
    cout << "Среднее число сравнений в Treap: " << (double)totalCompTreap / testKeys.size() << endl;
    cout << "Найдено ключей в списке: " << foundList << " из " << testKeys.size() << endl;
    cout << "Найдено ключей в Treap: " << foundTreap << " из " << testKeys.size() << endl;

    return 0;
}