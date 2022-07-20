#pragma once
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

template <class T>
class Heap{
	vector<T> a;
	int n = 0;
public:
	void insert(T item){
		a.push_back(item);
		n++;
		int i = n-1;
		while(i!=0 && a[i]<a[(i-1)/2]){
			swap(a[i], a[(i-1)/2]);
			i = (i-1)/2;
		}
	}

	int size(){
		return n;
	}

	T peek_min(){
		if(n==0){
			cout << "Heap is empty" << endl;
			exit(1);
		}
		return a[0];
	}

	T get_min(){
		if(n==0){
			cout << "Heap is empty" << endl;
			exit(1);
		}
		swap(a[0], a[n-1]);
		int i = 0;
		while(true){
			int next = -1;
			T m = a[i];
			if(2*i+1<n-1 && a[2*i+1]<m){
				m = a[2*i+1];
				next = 2*i+1;
			}
			if(2*i+2<n-1 && a[2*i+2]<m){	
				next = 2*i+2;	
			}
			if(next == -1)break;
			swap(a[i], a[next]);
			i = next;
		}
		T item = a.back();
		a.pop_back();
		n--;
		return item;
	}
	/*void display(){
		for(int x:a)cout << x << ' ';
			cout << endl;
	}*/
};
