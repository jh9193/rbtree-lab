#include "rbtree.h"

#include <stdlib.h>

void delete_postorder(rbtree *t, node_t *node);
void right_rotate(rbtree *t, node_t *node);
void left_rotate(rbtree *t, node_t *node);
void insert_fixup(rbtree *t, node_t *node);
node_t *find_successor(rbtree *t, node_t *node);
int rbtree_erase(rbtree *t, node_t *node);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
void rbtree_delete_fixup(rbtree *t, node_t* remove);
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n);
int put_arr_inorder(node_t *node, const rbtree *t, key_t *arr, int i);

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));

  nil->color = RBTREE_BLACK;

  p->nil = nil;
  p->root = p->nil;

  // TODO: initialize struct if needed
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  node_t *node = t->root;
  if(node != t->nil)
    delete_postorder(t, node);
  free(t->nil);
  free(t);
}

void delete_postorder(rbtree *t, node_t *node)
{
    if(node == t->nil) return;
    delete_postorder(t, node->left);
    delete_postorder(t, node->right);
    free(node);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));  //새 노드 동적 할당
  new_node->key = key;                                     //새 노드의 키 값 입력
  new_node->color = RBTREE_RED;                            //새 노드는 빨간색으로 삽입

  node_t *end = t->nil;                                    //end 노드는 nil노드
  node_t *current = t->root;                               //root부터 탐색

  while (current != t->nil)                                //새 노드가 들어갈 위치 찾기
  { end = current;                                      
    if(key < current->key)                                 //삽입할 키값이 현재노드의 키값보다 작으면,
        current = current->left;                           //현재노드의 왼쪽자식 탐색
    else
        current = current->right;                          //크다면 오른쪽자식 탐색
  }

  new_node->parent = end;                                  //새 노드의 부모노드를 end 노드로 설정
  if (end == t->nil)                                       //end 노드가 nil일 경우,
    t->root = new_node;                                    //새 노드는 root 노드
  else if (key < end->key)                                 //삽입하려는 값이 end노드의 값보다 작으면,
    end->left = new_node;                                  //end노드의 왼쪽자식으로 삽입
  else  
    end->right = new_node;                                 //크다면 오른쪽자식으로 삽입.

  new_node->left = t->nil;                                 //새 노드의 왼쪽, 오른쪽 자식을 nil노드로 설정
  new_node->right = t->nil;

  insert_fixup(t, new_node);                               //rbtree의 법칙에 위배되지 않도록 재배치 작업 실시

  return new_node;
//   return t->root;
}

void insert_fixup(rbtree *t, node_t *node)                      
{   
    node_t *uncle;                                              //삼촌노드 선언
    while (node->parent->color == RBTREE_RED)                   //부모노드가 빨간색일때 (#4번 법칙 위배)
    {
        if (node->parent == node->parent->parent->left)         //부모노드가 조부모 노드의 왼쪽자식일때,
        {
            uncle = node->parent->parent->right;                //삼촌노드는 조부모 노드의 오른쪽자식으로 설정.
            if (uncle->color == RBTREE_RED)                     //삼촌노드가 빨간색일때,                               //<경우 1번>
            {
                node->parent->color = RBTREE_BLACK;             //부모와 삼촌노드를 검정색으로 바꿔주고,
                uncle->color = RBTREE_BLACK;
                node->parent->parent->color = RBTREE_RED;       //조부모노드를 빨간색으로 바꿔주고
                node = node->parent->parent;                    //이후 조부모 노드를 기준으로 다시 재배치 작업 실시.
            }
            else                                                //삼촌노드가 검정색일때,
            {
                if (node == node->parent->right)                //새로 삽입한 노드가 부모노드의 오른쪽 자식일때(꺾인모양) //<경우 2번>
                {
                    node = node->parent;                        //새로 삽입한 노드의 부모노드를 기준으로,
                    left_rotate(t, node);                       //왼쪽으로 회전(꺾인 모양을 풀어주는 작업. 경우 3번으로 가기위한 작업)
                }                                               //새로 삽입한 노드가 부모노드의 왼쪽 자식일때            //<경우 3번>
                node->parent->color = RBTREE_BLACK;             //부모노드의 색깔을 검정색으로 바꿔주고,
                node->parent->parent->color = RBTREE_RED;       //조부모노드의 색깔을 빨간색으로 바꿔준 후,
                right_rotate(t, node->parent->parent);          //조부모노드를 기준으로 오른쪽으로 회전
            }
        }
        else                                                    //부모노드가 조부모 노드의 오른쪽 자식일 경우로, 위 경우의 오른쪽, 왼쪽을 바꾸어 진행
        {
            uncle = node->parent->parent->left;
            if (uncle->color == RBTREE_RED)
            {
                node->parent->color = RBTREE_BLACK;
                uncle->color = RBTREE_BLACK;
                node->parent->parent->color = RBTREE_RED;
                node = node->parent->parent;
            }
            else 
            {
                if (node == node->parent->left)
                {
                    node = node->parent;
                    right_rotate(t, node);
                }
                node->parent->color = RBTREE_BLACK;
                node->parent->parent->color = RBTREE_RED;
                left_rotate(t, node->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}

void left_rotate(rbtree *t, node_t *node)
{
    node_t *right = node->right;                //타겟 노드의 오른쪽 자식 선언
    node->right = right->left;                  //타겟 노드의 오른쪽 자식을 타겟 노드 오른쪽 자식의 왼쪽자식으로 설정 (1)
    if (right->left != t-> nil)                 //오른쪽 자식의 왼쪽자식이 있다면,
        right->left->parent = node;             //오른쪽 자식의 왼쪽자식의 부모를 타겟 노드로 설정 (1)과 양방향으로 연결 완료
    right->parent = node->parent;               //오른쪽 자식의 부모를 타겟 노드의 부모로 설정 (2)
    if (node->parent == t->nil)                 //타겟 노드가 root노드라면,
        t->root = right;                        //오른쪽 자식이 루트노드
    else if (node == node->parent->left)        //타겟 노드가 그 부모의 왼쪽자식이라면,
        node->parent->left = right;             //타겟 노드의 부모의 왼쪽자식을, 오른쪽자식으로 설정 (2)와 양방향 연결
    else                                        //오른쪽 자식이라면,
        node->parent->right = right;            //타겟 노드의 부모의 오른쪽자식을, 오른쪽자식으로 설정 (2)와 양방향 연결
    right->left = node;                         //오른쪽자식의 왼쪽 자식으로 타겟 노드를 설정 (3)
    node->parent = right;                       //타겟 노드의 부모로 오른쪽 자식을 설정 (3)과 양방향 연결
}

void right_rotate(rbtree *t, node_t *node)      //왼쪽 회전과 반대의 과정
{
    node_t *left = node->left;
    node->left = left->right;
    if (left->right != t-> nil)
        left->right->parent = node;
    left->parent = node->parent;
    if (node->parent == t->nil)
        t->root = left;
    else if (node == node->parent->right)
        node->parent->right = left;
    else
        node->parent->left = left;
    left->right = node;
    node->parent = left;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *current = t->root;           //root부터 탐색 시작
  while (current != t-> nil)           //현재 노드가 nil노드가 아니면 계속 반복문 수행
  {
    if (key == current->key)           //찾으려는 값과 현재 노드의 값이 같으면 현재 노드를 반환 
        return current;
    else if (key < current->key)       //찾으려는 값이 현재 노드의 값보다 작으면,
        current = current->left;       //현재 노드의 왼쪽 자식으로 내려간다
    else
        current = current->right;      //크다면, 현재노드의 오른쪽 자식으로 내려간다.
  }
  return NULL;                         //값을 찾지 못한다면 NULL값을 반환
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *target = t->root;             //시작 노드는 root노드
  while (target->left != t->nil)        //노드의 왼쪽자식이 nil일때까지 반복문 수행
  {
    target = target->left;              //한번 반복할때마다 노드의 왼쪽 자식으로 내려간다
  }
  
  return target;                        //반복문이 종료되었을 때 노드를 반환
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *target = t->root;             //시작 노드는 root노드
  while (target->right != t->nil)       //노드의 오른쪽자식이 nil일때까지 반복문 수행
  {
    target = target->right;             //한번 반복할때마다 노드의 오른쪽 자식으로 내려간다
  }
  
  return target;                        //반복문이 종료되었을 때 노드를 반환
}

node_t *find_successor(rbtree *t, node_t *node)     //삭제하고자 하는 노드의 위치를 대체할 노드 찾기 (오른쪽 서브트리의 최솟값 찾기)
{
    while (node->left != t->nil)                    //노드의 왼쪽자식이 nil일때 까지 반복문 수행
    {
        node = node->left;                          //한번 반복할때마다 노드의 왼쪽 자식으로 내려간다
    }
    
    return node;                                    //오른쪽 서브트리의 최솟값 반환
}

int rbtree_erase(rbtree *t, node_t *node) {
  // TODO: implement erase
  node_t *remove;                                       //실제 제거되는 노드
  node_t *delete = node;                                //삭제되는 키값을 가진 노드
  int delete_origin_color = delete->color;              //실제 삭제되는 색상을 판별하기 위한 정수형 선언
  if(node->left == t->nil)                              //노드의 왼쪽 자식이 nil노드일 때,
  {
    remove = node->right;                               //실제 제거되는 노드는 현재 노드의 오른쪽 자식
    rbtree_transplant(t, node, node->right); 
  }
  else if(node->right == t->nil)                        //노드의 오른쪽 자식이 nil노드일 때,
  {
    remove = node->left;                                //실제 제거되는 노드는 현재 노드의 왼쪽자식
    rbtree_transplant(t, node, node->left);
  }
  else                                                  //노드의
  {
    delete = find_successor(t, node->right);
    delete_origin_color = delete->color;
    remove = delete->right;
    if (delete->parent == node)
    {
        remove->parent = delete;
    }
    else
    {
        rbtree_transplant(t, delete, delete->right);
        delete->right = node->right;
        delete->right->parent = delete;
    }
    rbtree_transplant(t, node, delete);
    delete->left = node->left;
    delete->left->parent = delete;
    delete->color = node->color;
  }
  if (delete_origin_color == RBTREE_BLACK)
    rbtree_delete_fixup(t, remove);

  free(node);
  return 0;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
    if (u->parent == t->nil)           //제거되는 노드가 부모가 없을 때,
        t->root = v;                   //대체노드는 root노드
    else if (u == u->parent->left)     //제거되는 노드가 그 부모의 왼쪽 자식일때,
        u->parent->left = v;           //대체 노드의 부모를 제거 노드 부모의 왼쪽 자식으로 설정
    else
        u->parent->right = v;          //오른쪽 자식이라면 오른쪽 자식으로 설정
    
    v->parent = u->parent;             //대체노드의 부모를 제거노드의 부모로 설정 (양방향 연결)
}

void rbtree_delete_fixup(rbtree *t, node_t* remove)
{
    node_t *sibling;
    while (remove != t->root && remove->color == RBTREE_BLACK)
    {
        if (remove == remove->parent->left)
        {
            sibling = remove->parent->right;
            if (sibling->color == RBTREE_RED)
            {
                sibling->color = RBTREE_BLACK;
                remove->parent->color = RBTREE_RED;
                left_rotate(t, remove->parent);
                sibling = remove->parent->right;
            }
            if (sibling->left->color == RBTREE_BLACK && sibling->right->color == RBTREE_BLACK)
            {
                sibling->color = RBTREE_RED;
                remove = remove->parent;
            }
            else
            {
                if (sibling->right->color == RBTREE_BLACK)
                {
                    // sibling->left->color == RBTREE_BLACK; // 영향 미치는게 없다해서 주석처리
                    sibling->color = RBTREE_RED;
                    right_rotate(t, sibling);
                    sibling = remove->parent->right;
                }
                sibling->color = remove->parent->color;
                remove->parent->color = RBTREE_BLACK;
                sibling->right->color = RBTREE_BLACK;
                left_rotate(t, remove->parent);
                remove = t->root;
            }
        }
        else
        {
            sibling = remove->parent->left;
            if (sibling->color == RBTREE_RED)
            {
                sibling->color = RBTREE_BLACK;
                remove->parent->color = RBTREE_RED;
                right_rotate(t, remove->parent);
                sibling = remove->parent->left;
            }
            if (sibling->right->color == RBTREE_BLACK && sibling->left->color == RBTREE_BLACK)
            {
                sibling->color = RBTREE_RED;
                remove = remove->parent;
            }
            else
            {
                if (sibling->left->color == RBTREE_BLACK)
                {
                    // sibling->right->color == RBTREE_BLACK; // 영향 미치는게 없다해서 주석처리
                    sibling->color = RBTREE_RED;
                    left_rotate(t, sibling);
                    sibling = remove->parent->left;
                }
                sibling->color = remove->parent->color;
                remove->parent->color = RBTREE_BLACK;
                sibling->left->color = RBTREE_BLACK;
                right_rotate(t, remove->parent);
                remove = t->root;
            }
        }
    }
    remove->color = RBTREE_BLACK;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  if (t->root != t->nil)
  {
    put_arr_inorder(t->root, t, arr, 0);
  }
  return 0;
}

int put_arr_inorder(node_t *node, const rbtree *t, key_t *arr, int i)
{
    if(node == t->nil)
        return i;

    i = put_arr_inorder(node->left, t, arr, i);
    arr[i++] = node->key;
    i = put_arr_inorder(node->right, t, arr, i);
    return i;
}
