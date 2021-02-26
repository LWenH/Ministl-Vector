#ifndef MINISTL_T_VECTOR_H
#define MINISTL_T_VECTOR_H
#include <iostream>
#include "../vector.h"
using namespace std;
using namespace ministl;

#define FUN_VALUE(fun) do {                              \
  std::string fun_name = #fun;                           \
  std::cout << " " << fun_name << " : " << fun << "\n";  \
} while(0)

// 遍历输出容器
#define COUT(container) do {                             \
  std::string con_name = #container;                     \
  std::cout << " " << con_name << " :";                  \
  for (auto it : container)                              \
    std::cout << " " << it;                              \
  std::cout << "\n";                                     \
} while(0)

// 输出容器调用函数后的结果
#define FUN_AFTER(con, fun) do {                         \
  std::string fun_name = #fun;                           \
  std::cout << " After " << fun_name << " :\n";          \
  fun;                                                   \
  COUT(con);                                             \
} while(0)

void test() {

    std::cout << "[===============================================================]\n";
    std::cout << "[----------------- Run container test : vector -----------------]\n";
    std::cout << "[-------------------------- API test ---------------------------]\n";
    int a[] = { 1,2,3,4,5 };
    ministl::vector<int> v1;
    ministl::vector<int> v2(10);
    ministl::vector<int> v3(10, 1);
    ministl::vector<int> v4(a, a + 5);
    ministl::vector<int> v5(v2);
    ministl::vector<int> v6(std::move(v2));
    ministl::vector<int> v7{ 1,2,3,4,5,6,7,8,9 };
    ministl::vector<int> v8, v9, v10;
    v8 = v3;
    v9 = std::move(v3);
    v10 = { 1,2,3,4,5,6,7,8,9 };

    FUN_AFTER(v1, v1.assign(8, 8));
    FUN_AFTER(v1, v1.assign(a, a + 5));
    FUN_AFTER(v1, v1.emplace(v1.begin(), 0));
    FUN_AFTER(v1, v1.emplace_back(6));
    FUN_AFTER(v1, v1.push_back(6));
    FUN_AFTER(v1, v1.insert(v1.end(), 7));
    FUN_AFTER(v1, v1.insert(v1.begin() + 3, 2, 3));
    FUN_AFTER(v1, v1.insert(v1.begin(), a, a + 5));
    FUN_AFTER(v1, v1.pop_back());
    FUN_AFTER(v1, v1.erase(v1.begin()));
    FUN_AFTER(v1, v1.erase(v1.begin(), v1.begin() + 2));
//    FUN_AFTER(v1, v1.reverse());
    FUN_AFTER(v1, v1.swap(v4));
    FUN_VALUE(*v1.begin());
    FUN_VALUE(*(v1.end() - 1));
    FUN_VALUE(*v1.rbegin());
    FUN_VALUE(*(v1.rend() - 1));
    FUN_VALUE(v1.front());
    FUN_VALUE(v1.back());
    FUN_VALUE(v1[0]);
    FUN_VALUE(v1.at(1));
    int* p = v1.data();
    *p = 10;
    *++p = 20;
    p[1] = 30;
    std::cout << " After change v1.data() :" << "\n";
    COUT(v1);
    std::cout << std::boolalpha;
    FUN_VALUE(v1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.max_size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.resize(10));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.shrink_to_fit());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.resize(6, 6));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.shrink_to_fit());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.clear());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.reserve(5));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.reserve(20));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.shrink_to_fit());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    std::cout << "[----------------- End container test : vector -----------------]\n";
}
#endif //MINISTL_T_VECTOR_H
