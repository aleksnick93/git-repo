#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

//Это параллельная ветка
//для различных улучшений
class B_tree
{
public:
	B_tree (); //Конструктор "пустого" B-дерева
	bool erase();
	~B_tree (); //Деструктор B-дерева
	void add (int key); //Добавление ключа key в структуру B-дерева
	bool search (int key); //Поиск ключа в B-дереве (true - найден, false - не найден)
	void printTree(); //Обход и вывод B-дерева на печать способом, используемым при составлении оглавления книг
	void printSorted();
private:
	struct Page
	{
		int RAmount; //Фактическое число элементов на странице
		Page* p0; //Указатель на самого левого потомка страницы
		static const int items = 4;
		struct Item
		{
			int key; //Значение одного ключа страницы
			Page* p; //Указатель на страницу-потомок
		} key_pt[4]; //Массив, хранящий значения ключей страницы и указатели на потомков страницы
	} *root; //Указатель на корень B-дерева
 
	void search_add_key (int key, Page *a, bool& grow, Page::Item& v);
//Рекурсивная функция, выполняющая добавление ключа key в структуру B-дерева; a - указатель на страницу, grow - "B-дерево стало выше", v - передаваемый вверх элемент
	void print_b_tree (Page *a, int level);
//Рекурсивная фунцкия, выполняющая печать B-дерева с уровня level
	bool search_key (int key, Page *a);
//Рекурсивная функция, выполняющая поиск ключа в B-дереве (true - ключ найден, false - не найден)
	void destroy (Page *a); //"Деструктор" вершины a B-дерева
	void printRecSorted(Page* page);
};
 
B_tree::B_tree()
{
	root = NULL;
}

B_tree::~B_tree()
{
    destroy(root);
    root = NULL;
}

bool B_tree::erase()
{
    if(root==NULL) 
    {   
        return false;
    }
    destroy(root);
    root = NULL;
    return true;
}
 
void B_tree::destroy (Page* page)
{
	if (page) {
		destroy(page->p0);
		for (int i = 0; i < page->RAmount; i++)
			destroy(page->key_pt[i].p);
		delete page;
	}
}
 
void B_tree::add (int key)
{
    bool grow = false;
    Page::Item u;
    search_add_key(key, root, grow, u);
    if (grow)
    {
        Page* bufferRoot = root;
        root = new Page;
        root->p0 = NULL;
        root->key_pt[0].p = NULL;
        root->key_pt[1].p = NULL;
        root->key_pt[2].p = NULL;
        root->key_pt[3].p = NULL;
        root->RAmount = 1;
        root->p0 = bufferRoot;
        root->key_pt[0] = u;
    }
}
 
void B_tree::search_add_key (int key, Page *a, bool& grow, Page::Item& v)
{
    short i, L, R;
    Page *b;
    Page::Item u;
 
    if (a == NULL)
    {
        grow = true;
        v.key = key;
		v.p = NULL;
		return;
    }
    else
    {
        L = 0;
        R = a->RAmount;
        while (L < R)
        {
            i = (L+R)/2;
            if (a->key_pt[i].key <= key)
                L = i+1;
            else
                R = i;
        }
        R--;
        if ((R >= 0) && (a->key_pt[R].key == key))
        	return;
        else
        {
            if (R == -1)
                search_add_key (key, a->p0, grow, u);
            else
                search_add_key (key, a->key_pt[R].p, grow, u);
            if (grow == true)
            {
                if (a->RAmount < 4)
                {
                    grow = false;
                    a->RAmount++;
                    for (i = a->RAmount-1; i >= R+2; i--)
                    a->key_pt[i] = a->key_pt[i-1];
                    a->key_pt[R+1] = u;
                }
                else
                {
                    b = new Page;
                    b->p0 = NULL;
                    b->key_pt[0].p = NULL;
                    b->key_pt[1].p = NULL;
                    b->key_pt[2].p = NULL;
                    b->key_pt[3].p = NULL;
                    if (R <= 1)
                    {
                        if (R == 1)
                            v = u;
                        else
                        {
                            v = a->key_pt[1];
                            for (i=1; i>=R+1; i--)
                            a->key_pt[i] = a->key_pt[i-1];
                            a->key_pt[R+1] = u;
                        }
                        for (i=0; i<=1; i++)
                        b->key_pt[i] = a->key_pt[i+2];
                    }
                    else
                    {
                        R = R-2;
                        v = a->key_pt[2];
                        for (i=0; i<=R-1; i++)
                        b->key_pt[i] = a->key_pt[i+3];
                        b->key_pt[R] = u;
                        for (i=R+1; i<=1; i++)
                        b->key_pt[i] = a->key_pt[i+2];
                    }
                    a->RAmount = 2;
                    b->RAmount = 2;
                    b->p0 = v.p;
                    v.p = b;
                }
            }
        }
    }
}
 
bool B_tree::search (int key)
{
    if (!root)
        return false;
    Page* current = root;
	while (current) {
		int left = 0;
		int right = current->RAmount;
		while (left < right) {
			int i = (left + right) / 2;
			if (key <= current->key_pt[i].key)
				right = i;
			else
				left = i + 1;
		}
		if ((right < current->RAmount) && (current->key_pt[right].key == key))
			return true;
		if (right == 0)
			current = current->p0;
		else
			current = current->key_pt[right-1].p;
	}
	return false;
}
 
void B_tree::printTree()
{
    if(root==NULL)
    {
        std::cout<< "Tree is empty!";
        return;
    }
	print_b_tree(root, 0);
}
  
void B_tree::print_b_tree (Page *a, int level)
{
    using namespace std;
    short i;
 
    if (a != NULL)
    {
        for (i=0; i < level; i++) 
            cout << "---";
        for (i=0; i < a->RAmount; i++)
        {
            cout << a->key_pt[i].key;
            if (i != a->RAmount-1)
                cout << ", ";
        }
        cout << endl;
        print_b_tree (a->p0, level+1);
        for (i=0; i < a->RAmount; i++)
            print_b_tree (a->key_pt[i].p, level+1);
    }
}

void B_tree::printSorted() {
	if (!root) {
		cout << "Tree is empty!";
		return;
	}
	printRecSorted(root);
}

void B_tree::printRecSorted(Page* page) {
	if (!page)
		return;
	printRecSorted(page->p0);
	for (int i = 0; i < page->RAmount; i++) {
		cout << page->key_pt[i].key << ' ';
		printRecSorted(page->key_pt[i].p);
	}
}
 
int main() {
	B_tree tree;
	cout << "Set up tree set" << endl;
	srand(time(NULL));
	tree.erase();
	for (int i = 0; i < 100; i++)
		tree.add(rand() % 150);
	tree.printTree();
	cout << endl;
	tree.printSorted();
	cout << endl;
	for (int i = 0; i < 10; i++) {
		cout << i;
		if (!tree.search(i))
			cout << " not";
		cout << " found" << endl;
	}
}
