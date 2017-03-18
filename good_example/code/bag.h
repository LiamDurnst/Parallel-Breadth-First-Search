#ifndef BAG_H_
#define BAG_H_

#include <cilki/hyperobject.h>
#include <cilk.h>
#include <vector>

extern "C++" {
class _bag;
class _bag
{
public:
	_bag(std::vector<int> v);
	~_bag();
	void insert(int v);
	void merge(_bag *other);
	_bag *split();
	int size();
	std::vector<int> dump();
	void cleanup();

//private:
	class pennant;
	class pennant
	{
	public:
		pennant(std::vector<int> v, int k);
		~pennant();
		void cleanup();
		void merge(pennant *p);
		void dump(std::vector<int> *vec);
		int val, k;

		class p_node;
		class p_node
		{
		public:
			p_node *kids[2];
			int val;
			p_node(std::vector<int> &v, int k);
			~p_node();
			void cleanup();
			void dump(std::vector<int> *vec);
		};
		p_node *tree;
	};


	void merge(pennant *p);

	std::vector<pennant *> arr;
};

struct bag : cilk::monoid_base<_bag *>
{
	static void reduce(_bag **l, _bag **r);
	static void identity(_bag **p);
};
}

#endif

