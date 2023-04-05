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
  // x의 오른쪽 자식 y설정
  node_t *y = x -> right;

  // y의 왼쪽 서브트리를 x의 오른쪽 서브트리로 옮기기
  x -> right = y -> left;
  if (y -> left != t -> nil) {
    y -> left -> parent = x;
  } 

  // y를 x의 부모 노드로 만들고 연결 수정
  y -> parent = x -> parent;
  if (x -> parent == t -> nil) {
    t -> root = y;
  } else if (x == x -> parent -> left) {
    x -> parent -> left = y;
  } else {
    x -> parent -> right = y;
  }

  // x를 y의 왼쪽 자식 노드로 만들기
  y -> left = x;
  x -> parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  // x의 왼쪽 자식 y 설정
  node_t *y = x -> left;

  // y의 오른쪽 서브트리를 x의 왼쪽 서브트리로 옮기기
  x -> left = y -> right;
  if (y -> right != t -> nil) {
    y -> right -> parent = x;
  } 

  // y를 x의 부모 노드로 만들고 연결 수정
  y -> parent = x -> parent;
  if (x -> parent == t -> nil) {
    t -> root = y;
  } else if (x == x -> parent -> right) {
    x -> parent -> right = y;
  } else {
    x -> parent -> left = y;
  }

  // x를 y의 오른쪽 자식 노드로 만들기
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

  node_t *parent_node = t -> nil;
  node_t *child_node = t -> root;
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
  } 
  // 2. key 값이 작다면 왼쪽 자식으로 삽입한다
  else if (new_node -> key < parent_node -> key) {
    parent_node -> left = new_node;
  } 
  // 3. key 값이 크다면 오른쪽 자식으로 삽입한다
  else {
    parent_node -> right = new_node;
  }

  // 신규 노드 좌우 자식 nil node 설정, 색상 red 설정
  new_node -> left = t -> nil;
  new_node -> right = t -> nil;
  new_node -> color = RBTREE_RED;

  // rbtree 수정하는 함수 호출하기
  rbtree_insert_fixup(t, new_node);

  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current = t -> root;

  while (current != t -> nil && key != current -> key) {
    if (key < current -> key) {
      // 현재 값보다 탐색하려는 값이 작으면 왼쪽 자식으로 가서 탐색한다
      current = current -> left;
    } else  {
      // 그 반대의 경우 오른쪽 자식으로 가서 탐색한다
      current = current -> right;
    } 
  }
  // key를 찾았으면 key 반환, 그렇지 않을 경우 NULL 반환
  if (key == current -> key) {
    return current;
  } else {
    return NULL;
  }
}

node_t *rbtree_min(const rbtree *t) {
  // 빈 트리
  if (t -> root == t -> nil) {
    return NULL;
  }

  // 가장 왼쪽 끝으로 탐색한다
  node_t *current = t -> root;
  while (current -> left != t -> nil) {
    current = current -> left;
  }
  return current;
}

node_t *rbtree_max(const rbtree *t) {
  // 빈 트리
  if (t -> root == t-> nil) {
    return NULL;
  }
  // 가장 오른쪽 끝으로 탐색한다
  node_t *current = t -> root;
  while (current -> right != t -> nil) {
    current = current -> right;
  }
  return current;
}

void rbtree_transplant(rbtree *t, node_t *target, node_t * replace) {
  // target - 삭제할 노드, replace - 대체할 노드
  // 1. 삭제할 노드의 부모노드가 nil일 때 == 삭제할 노드가 root 노드라면
  if (target -> parent == t->nil){
    t->root = replace; // v가 root 노드로 대체된다
  }
  // 2. 삭제할 노드가 부모노드의 왼쪽 자식
  else if (target == target->parent->left){
    target->parent->left = replace;
  } 
  // 3. 삭제할 노드가 부모노드의 오른쪽 자식
  else {
    target->parent->right = replace;
  }
  // 삭제할 노드의 부모노드를 대체할 노드 부모노드로 설정
  replace->parent = target->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *target) {
  while (target != t -> root && target -> color == RBTREE_BLACK) {
    if (target == target -> parent -> left) {
      node_t *brother = target -> parent -> right; // 형제 노드
      // Case 1. 형제노드가 red - 부모와 형제의 색을 바꾼 후 부모를 기준으로 왼쪽 회전
      if (brother -> color == RBTREE_RED) {
        brother -> color = RBTREE_BLACK;
        target -> parent -> color = RBTREE_RED;
        left_rotate(t, target -> parent);
        brother = target -> parent -> right;
      }
      // Case 2. 형제의 두 자녀가 모두 black - 부모인 형제노드를 red로 칠하고 target을 target의 parent로 선언
      if (brother -> left -> color == RBTREE_BLACK && brother -> right -> color == RBTREE_BLACK) {
        brother -> color = RBTREE_RED;
        target = target -> parent; // while loop을 통해 위 노드로 올라가면서 탐색한다
      } else {
        // Case 3. 형제의 왼쪽 자식은 red, 오른쪽 자식은 black
        if (brother -> right -> color == RBTREE_BLACK) {
          // 형제의 왼쪽 자식과 형제의 색상을 바꾼 후 brother에 대해 우회전을 한다
          brother -> left -> color = RBTREE_BLACK;
          brother -> color = RBTREE_RED;
          right_rotate(t, brother);
          brother = target -> parent -> right;
        }
        // Case 4. 형제의 오른쪽 자식이 red
        // 형제와 target 부모 색상을 바꾼 후 target 부모를 기준으로 좌회전
        brother -> color = target -> parent -> color;
        target -> parent -> color = RBTREE_BLACK;
        brother -> right -> color = RBTREE_BLACK;
        left_rotate(t, target->parent);
        target = t -> root; // while 문 탈출
      }
    }
    // 좌우반전 
    else {
      node_t *brother = target -> parent -> left;
      if (brother -> color == RBTREE_RED) {
        brother -> color = RBTREE_BLACK;
        target -> parent -> color = RBTREE_RED;
        right_rotate(t, target -> parent);
        brother = target -> parent -> left;
      }
      if (brother -> right -> color == RBTREE_BLACK && brother -> left -> color == RBTREE_BLACK) {
        brother -> color = RBTREE_RED;
        target = target -> parent;
      } else {
        if (brother -> left -> color == RBTREE_BLACK) {
          brother -> right -> color = RBTREE_BLACK;
          brother -> color = RBTREE_RED;
          left_rotate(t, brother);
          brother = target -> parent -> left;
        }
        brother -> color = target -> parent -> color;
        target -> parent -> color = RBTREE_BLACK;
        brother -> left -> color = RBTREE_BLACK;
        right_rotate(t, target->parent);
        target = t -> root;
      }
    }
  }
  target -> color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *target) {
  node_t *tmp = target;  // 삭제할 노드를 tmp에 복사
  node_t *replace;  // 대체할 노드
  color_t tmp_original_color = tmp -> color;
  // 1. 왼쪽 자식이 없는 경우
  if (target -> left == t -> nil){
    replace = target -> right; // target의 오른쪽 자식을 replace로 설정
    rbtree_transplant(t, target, target -> right);  //오른쪽 자식을 target에 이식한다
  } 
  // 2. 오른쪽 자식이 없는 경우
  else if (target -> right == t -> nil){
    replace = target -> left;
    rbtree_transplant(t, target, target -> left);    
  } 
  // 3. 양쪽 모두 자식이 있는 경우
  else {
    // successor를 찾는다 (오른쪽 자식트리에서 가장 작은 값)
    tmp = target -> right;
    while (tmp -> left != t -> nil){
      tmp = tmp -> left;
    }
    tmp_original_color = tmp -> color;    // 삭제되는 색은 successor의 색상이다
    replace = tmp -> right;       // replace를 successor의 오른쪽 자식으로 설정한다(successor은 왼쪽 자식을 가질 수 없다)
    
    // 1. tmp가 target의 오른쪽 자식인 경우
    // target이 tmp의 원래 부모인 경우 (target의 자식이 둘이고, tmp가 target의 오른쪽 자식인 경우에만 발생)
    if(tmp -> parent == target){
      replace -> parent = tmp;
    }  
    else {
      rbtree_transplant(t, tmp, tmp -> right);
      tmp -> right = target -> right;
      tmp -> right -> parent = tmp;
    }
    // 2. y가 p의 왼쪽 자식인 경우
    rbtree_transplant(t, target, tmp);
    tmp -> left = target -> left;
    tmp -> left -> parent = tmp;
    tmp -> color = target -> color;
  }
  // 삭제되는 색상이 black이라면 속성위반 여부를 확인해야 한다
  if (tmp_original_color == RBTREE_BLACK){
    rbtree_delete_fixup(t, replace);
  }
  free(target);
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
