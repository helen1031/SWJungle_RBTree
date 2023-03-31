#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void) {
  // 트리 초기화
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));
  // nil_node의 색상은 black 이다
  nil_node -> color = RBTREE_BLACK;

  p -> nil = nil_node;
  p -> root = nil_node;

  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x -> right;
  x -> right = y -> left;
  if (y -> left != t -> nil) {
    y -> left -> parent = x;
  } 
  y -> parent = x -> parent;

  if (x -> parent == t -> nil) {
    t -> root = y;
  } else if (x == x -> parent -> left) {
    x -> parent -> left = y;
  } else {
    x -> parent -> right = y;
  }
  y -> left = x;
  x -> parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x -> left;
  x -> left = y -> right;
  if (y -> right != t -> nil) {
    y -> right -> parent = x;
  } 
  y -> parent = x -> parent;

  if (x -> parent == t -> nil) {
    t -> root = y;
  } else if (x == x -> parent -> right) {
    x -> parent -> right = y;
  } else {
    x -> parent -> left = y;
  }
  y -> right = x;
  x -> parent = y;
}

void rbtree_insert_fixup(rbtree *t, node_t *new_node) {
  // 총 6가지 경우 탐색
  while (new_node -> parent -> color == RBTREE_RED ) {
    // 부모노드가 조부모의 왼쪽 자식이라면
    if (new_node -> parent == new_node -> parent -> parent -> left) {
      node_t *uncle_node = new_node -> parent -> parent -> right;
      // 삼촌의 색상이 red 인 경우 
      // Case 1. 부모도 red, 삼촌도 red
      if (uncle_node -> color == RBTREE_RED) {
        new_node -> parent -> color = RBTREE_BLACK;         // 부모를 black으로
        uncle_node -> color = RBTREE_BLACK;                 // 삼촌도 black으로
        new_node -> parent -> parent -> color = RBTREE_RED; // 조부모는 red로
        new_node = new_node -> parent -> parent;            // 조부모를 기준으로 재탐색

      // 삼촌의 색상이 black인 경우
      } else {
        // Case 2. 삽입된 red가 부모의 오른쪽 자녀
        if (new_node == new_node -> parent -> right) {
          new_node = new_node -> parent;
          left_rotate(t, new_node); // 부모를 기준으로 왼쪽으로 회전 진행한다
        }
        // Case 3. 삽입된 red가 부모의 왼쪽 자녀
        // 부모(red), 조부모(black) 색상을 각각 black, red로 변경 후 
        // right_rotate를 진행한다
        new_node -> parent -> color = RBTREE_BLACK;
        new_node -> parent -> parent -> color = RBTREE_RED;
        right_rotate(t, new_node -> parent -> parent);
      } 
    // 부모노드가 조부모의 오른쪽 자식이라면
    } else {
      // 삼촌 노드는 조부모의 왼쪽 자식
      node_t *uncle_node = new_node -> parent -> parent -> left;
      // Case 1. 부모도 red, 삼촌도 red
      if (uncle_node -> color == RBTREE_RED) {
        new_node -> parent -> color = RBTREE_BLACK;
        uncle_node -> color = RBTREE_BLACK;
        new_node -> parent -> parent -> color = RBTREE_RED;
        new_node = new_node -> parent -> parent;
      // 삼촌이 black 이라면
      } else {
        // Case 2. 신규 red가 부모노드의 왼쪽 자식이라면
        if (new_node == new_node -> parent -> left) {
          new_node = new_node -> parent;
          right_rotate(t, new_node); // 부모를 기준으로 오른쪽 회전 진행
        }
        // Case 3. 신규 red가 부모노드의 오른쪽 자식이라면
        new_node -> parent -> color = RBTREE_BLACK;
        new_node -> parent -> parent -> color = RBTREE_RED;
        left_rotate(t, new_node -> parent -> parent); 
      }
    }
  }
  // root 노드의 색상을 black으로 변경
  t -> root -> color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // 삽입하려는 new_node 초기화
  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  new_node -> key = key;

  node_t *parent_node = t -> nil;        // y
  node_t *child_node = t -> root;    // x
  // new_node의 삽입 위치 탐색 : 이진탐색트리 특성 이용하여 key 값 비교하기
  while (child_node != t -> nil) {
    parent_node = child_node;
    if (new_node -> key < child_node -> key ) {
      child_node = child_node -> left;
    } else {
      child_node = child_node -> right;
    }
  }
  new_node -> parent = parent_node;

  // 1. root가 비어있는 상태
  if (parent_node == t -> nil) {
    t -> root = new_node;
  } else if (new_node -> key < parent_node -> key) {
    parent_node -> left = new_node;
  } else {
    parent_node -> right = new_node;
  }

  new_node -> left = t -> nil;
  new_node -> right = t -> nil;
  new_node -> color = RBTREE_RED;

  // rbtree 수정하는 함수 호출하기
  rbtree_insert_fixup(t, new_node);

  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t -> root;
  while (x != t -> nil && key != x -> key) {
    if (key < x -> key) {
      x = x -> left;
    } else  {
      x = x -> right;
    } 
  }

  if (key == x -> key) {
    return x;
  } else {
    return NULL;
  }
}

node_t *rbtree_min(const rbtree *t) {
  if (t -> root == t -> nil) {
    return NULL;
  }
  node_t *current = t -> root;
  while (current -> left != t -> nil) {
    current = current -> left;
  }
  return current;
}

node_t *rbtree_max(const rbtree *t) {
  if (t -> root == t-> nil) {
    return NULL;
  }
  node_t *current = t -> root;
  while (current -> right != t -> nil) {
    current = current -> right;
  }
  return current;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t * v) {
  if (u->parent == t->nil){
    t->root = v;
  } else if (u == u->parent->left){
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x) {
  while (x != t -> root && x -> color == RBTREE_BLACK) {
    if (x == x -> parent -> left) {
      node_t *w = x -> parent -> right;
      if (w -> color == RBTREE_RED) {
        w -> color = RBTREE_BLACK;
        x -> parent -> color = RBTREE_RED;
        left_rotate(t, x -> parent);
        w = x -> parent -> right;
      }
      if (w -> left -> color == RBTREE_BLACK && w -> right -> color == RBTREE_BLACK) {
        w -> color = RBTREE_RED;
        x = x -> parent;
      } else {
        if (w -> right -> color == RBTREE_BLACK) {
          w -> left -> color = RBTREE_BLACK;
          w -> color = RBTREE_RED;
          right_rotate(t, w);
          w = x -> parent -> right;
        }
        w -> color = x -> parent -> color;
        x -> parent -> color = RBTREE_BLACK;
        w -> right -> color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t -> root;
      }
    } else {
      node_t *w = x -> parent -> left;
      if (w -> color == RBTREE_RED) {
        w -> color = RBTREE_BLACK;
        x -> parent -> color = RBTREE_RED;
        right_rotate(t, x -> parent);
        w = x -> parent -> left;
      }
      if (w -> right -> color == RBTREE_BLACK && w -> left -> color == RBTREE_BLACK) {
        w -> color = RBTREE_RED;
        x = x -> parent;
      } else {
        if (w -> left -> color == RBTREE_BLACK) {
          w -> right -> color = RBTREE_BLACK;
          w -> color = RBTREE_RED;
          left_rotate(t, w);
          w = x -> parent -> left;
        }
        w -> color = x -> parent -> color;
        x -> parent -> color = RBTREE_BLACK;
        w -> left -> color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t -> root;
      }
    }
  }
  x -> color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y = p;
  node_t *x;
  color_t y_original_color = y -> color;
  if (p -> left == t -> nil){
    x = p -> right;
    rbtree_transplant(t, p, p -> right);
  } else if (p -> right == t -> nil){
    x = p -> left;
    rbtree_transplant(t, p, p -> left);    
  } else {
    y = p -> right;
    while (y -> left != t -> nil){
      y = y -> left;
    }
    y_original_color = y -> color;
    x = y -> right;
    if(y -> parent == p){
      x -> parent = y;
    } else {
      rbtree_transplant(t, y, y -> right);
      y -> right = p -> right;
      y -> right -> parent = y;
    }
    rbtree_transplant(t, p, y);
    y -> left = p -> left;
    y -> left -> parent = y;
    y -> color = p -> color;
  }
  if (y_original_color == RBTREE_BLACK){
    rbtree_delete_fixup(t, x);
  }
  free(p);
  return 0;
}

void convert_to_inorder(const rbtree *t, node_t *current, key_t *arr, int *index, const size_t n) {
  if (current == t -> nil) {
    return;
  }
  convert_to_inorder(t, current -> left, arr, index, n);
  if (*index < n) {
    arr[*index] = current -> key;
    *index += 1;
  } else {
    return;
  }
  convert_to_inorder(t, current -> right, arr, index, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t -> root == t -> nil) {
    return 0;
  }

  int index = 0;
  convert_to_inorder(t, t-> root, arr, &index, n);

  return 0;
}
