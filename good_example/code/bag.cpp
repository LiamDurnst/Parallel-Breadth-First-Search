#include "bag.h"
#include <stdio.h>

_bag::_bag(std::vector<int> v)
{
	arr.resize(43);
	for(int i = 0; i < 43; i++)
		arr[i] = NULL;
	// Loop through the 1s in binary rep of size of v
	while(v.size() != 0)
	{
		int dig = sizeof(int) * 8 - __builtin_clz(v.size()) - 1;
		arr[dig] = new pennant(v, dig);
	}
}

_bag::~_bag()
{
	for(std::vector<pennant *>::iterator it = arr.begin();
			it != arr.end();
			it++)
	{
		if(*it != NULL)
			delete *it;
	}
}

void _bag::insert(int v)
{
	std::vector<int> vec(1);
	vec[0] = v;
	pennant *p = new pennant(vec, 0);
	merge(p);
}

void _bag::merge(_bag *other)
{
	for(std::vector<pennant *>::iterator it = other->arr.begin();
			it != other->arr.end();
			it++)
	{
		if((*it) != NULL)
			this->merge(*it);
	}
}

_bag *_bag::split()
{
	_bag *rval = new _bag(std::vector<int>());
	std::vector<pennant::p_node *> garbage;
	pennant *prev = NULL, *swp = NULL; // for swapping

	for(int i = arr.size() - 1; i > 0; i--)
	{
		if(arr[i] != NULL)
		{
			pennant *pen = this->arr[i];
			std::vector<int> tmp;
			tmp.push_back(this->arr[i]->tree->val);
			rval->arr[i-1] = new pennant(tmp, 0);
			rval->arr[i-1]->k = i - 1;
			rval->arr[i-1]->tree = this->arr[i]->tree->kids[0];

			garbage.push_back(this->arr[i]->tree);

			arr[i]->k--;
			arr[i]->tree = arr[i]->tree->kids[1];

			swp = arr[i];
		}

		arr[i] = prev;
		prev = swp;
		swp = NULL;
	}

	if(prev != NULL)
	{
		this->insert(prev->val);
		delete prev;
		prev = NULL;
	}

	// Need to clean up now, though
	for(std::vector<pennant::p_node *>::iterator it = garbage.begin();
			it != garbage.end();
			it++)
	{
		delete *it;
	}

	return rval;
}

int _bag::size()
{
	int rval = 0, mul = 1;
	for(std::vector<pennant *>::iterator it = arr.begin();
			it != arr.end();
			it++)
	{
		if((*it) != NULL)
			rval += mul;
		mul *= 2;
	}

	return rval;
}

std::vector<int> _bag::dump()
{
	std::vector<int> rval;
	int i = 0;
	rval.resize(0);

	for(std::vector<_bag::pennant *>::iterator it = arr.begin();
			it != arr.end();
			it++)
	{
		if((*it) != NULL)
		{
			if(i != (*it)->k)
			{
				printf("\n\n");
				printf("Adding in %d, which claims to have k of %d.\n", i, (*it)->k);
				printf("This is probably bad.\n\n\n");
			}
			(*it)->dump(&rval);
		}

		i++;
	}

	return rval;
}

void _bag::cleanup()
{
	for(std::vector<_bag::pennant *>::iterator it = arr.begin();
			it != arr.end();
			it++)
	{
		if((*it) != NULL)
		{
			(*it)->cleanup();
			delete *it;
		}

		*it = NULL;
	}
}

void _bag::merge(pennant *p)
{
	int carry, dig = p->k;
	do {
		if(arr[dig] == NULL)
		{
			carry = 0;
			arr[dig] = p;
		}
		else
		{
			p->merge(arr[dig]);
			delete arr[dig];
			arr[dig] = NULL;
			dig++;
			carry = 1;
		}
	} while(carry);
}

_bag::pennant::pennant(std::vector<int> v, int k)
{
	val = v.back();
	v.pop_back();

	this->k = k;
	if(k != 0)
		tree = new p_node(v, k);
	else
		tree = NULL;
}

_bag::pennant::~pennant()
{
	// Do nothing
}

void _bag::pennant::cleanup()
{
	if(k > 0)
	{
		tree->cleanup();
		delete tree;
		tree = NULL;
	}
}

void _bag::pennant::merge(pennant *p)
{
	if(p == NULL)
		printf("Mishap.\n");
	if(p->k != this->k)
		printf("Terrible.\n");

	// If we're in this method....
	// hopefully p->k == this->k.
	std::vector<int> temp_vec;
	temp_vec.push_back(p->val);
	p_node *root = new p_node(temp_vec, 0);

	root->kids[0] = this->tree;
	root->kids[1] = p->tree;

	this->tree = root;

	k++;
}

void _bag::pennant::dump(std::vector<int> *vec)
{
	vec->push_back(val);
	if(k > 0)
		tree->dump(vec);
}

_bag::pennant::p_node::p_node(std::vector<int> &v, int k)
{
	this->val = v.back();
	v.pop_back();

	if(k > 0)
	{
		k--;
		kids[0] = new p_node(v, k);
		kids[1] = new p_node(v, k);
	}
	else
	{
		kids[0] = kids[1] = NULL;
	}
}

_bag::pennant::p_node::~p_node()
{ }

void _bag::pennant::p_node::cleanup()
{
	if(kids[0] != NULL)
	{
		kids[0]->cleanup();
		kids[1]->cleanup();

		delete kids[0];
		delete kids[1];

		kids[0] = NULL;
		kids[1] = NULL;
	}
}

void _bag::pennant::p_node::dump(std::vector<int> *vec)
{
	vec->push_back(val);
	if(kids[0] != NULL)
	{
		// If one is, both are...
		kids[0]->dump(vec);
		kids[1]->dump(vec);
	}
}

void bag::reduce(_bag **left, _bag **right)
{
	(*left)->merge(*right);
}

void bag::identity(_bag **p)
{
	*p = new _bag(std::vector<int>());
}

