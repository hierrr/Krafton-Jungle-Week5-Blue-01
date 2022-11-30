#include "rbtree.h"
#include <stdlib.h>

// init
rbtree *new_rbtree(void)
{
    rbtree *t = NULL;
    node_t *nil = NULL;

    t = (rbtree *)calloc(1, sizeof(rbtree));
    if (!t)
        return (NULL);  // err: malloc failed
    nil = (node_t *)calloc(1, sizeof(node_t));
    if (!nil)
        return (NULL);  // err: malloc failed
    nil->color = RBTREE_BLACK;
    t->nil = nil;
    t->root = t->nil;
    return (t);
}

// fin
static void delete_node(rbtree *t, node_t *x)
{
    if (x == t->nil)
        return ;
    delete_node(t, x->left);
    delete_node(t, x->right);
    free(x);
    x = NULL;
}

void delete_rbtree(rbtree *t)
{
    if (!t)
        return ;
    delete_node(t, t->root);
    free(t->nil);
    t->nil = NULL;
    free(t);
    t = NULL;
}

// rot
static void rbtree_rotate_left(rbtree *t, node_t *x)
{
    node_t *y = NULL;

    y = x->right;
    // move y.l to x.r
    x->right = y->left;
    if (y->left != t->nil)
        y->left->parent = x;
    // set y.p as x.p
    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else // (x == x->parent->right)
        x->parent->right = y;
    // set x as y.l
    y->left = x;
    x->parent = y;
}

static void rbtree_rotate_right(rbtree *t, node_t *x)
{
    node_t *y = NULL;

    y = x->left;
    // move y.r to x.l
    x->left = y->right;
    if (y->right != t->nil)
        y->right->parent = x;
    // set y.p as x.p
    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else // (x == x->parent->right)
        x->parent->right = y;
    // set x as y.r
    y->right = x;
    x->parent = y;
}

// ins
static void rbtree_insert_fixup(rbtree *t, node_t *z)
{
    node_t *y = NULL;

    while (z->parent->color == RBTREE_RED)
    {
        // case 1-3: z's parent is left child
        if (z->parent == z->parent->parent->left)
        {
            y = z->parent->parent->right;
            // case 1: z's uncle y is red
            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            // case2: z's uncle y is balck, z is right child
            else // (y->color == RBTREE_BLACK)
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    rbtree_rotate_left(t, z);
                }
                // case 3: z's uncle y is black, z is right child
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                rbtree_rotate_right(t, z->parent->parent);
            }
        }
        // case 4-6: z's parent is right child
        else // (z->parent == z->parent->parent->right)
        {
            y = z->parent->parent->left;
            // case 4: z's uncle y is red
            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            // case 5: z's uncle y is black, z is right child
            else // (y->color == RBTREE_BLACK)
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    rbtree_rotate_right(t, z);
                }
                // case 6: z's uncle y is black, z is right child
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                rbtree_rotate_left(t, z->parent->parent);
            }
        }
    }
    t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
    node_t *x = NULL;
    node_t *y = NULL;
    node_t *z = NULL;

    y = t->nil;
    x = t->root;
    z = (node_t *)calloc(1, sizeof(node_t));
    if (!z)
        return (NULL);  // err: malloc failed
    z->key = key;
    while (x != t->nil)
    {
        y = x;
        if (z->key < x->key)
            x = x->left;
        else // (z->key >= x->key)
            x = x->right;
    }
    z->parent = y;
    if (y == t->nil)
        t->root = z;
    else if (z->key < y->key)
        y->left = z;
    else // (z->key >= y->key)
        y->right = z;
    // set z data
    z->left = t->nil;
    z->right = t->nil;
    z->color = RBTREE_RED;
    rbtree_insert_fixup(t, z);
    return (z);
}

// peek
node_t *rbtree_find(const rbtree *t, const key_t key)
{
    node_t *cur = NULL;

    cur = t->root;
    while (cur != t->nil && cur->key != key)
    {
        if (cur->key > key)
            cur = cur->left;
        else // (cur->key <= key)
            cur = cur->right;
    }
    if (cur->key == key)
        return (cur);
    return(NULL);   // err: wrong key
}

node_t *rbtree_min(const rbtree *t)
{
    node_t *cur = NULL;

    cur = t->root;
    while (cur->left != t->nil)
        cur = cur->left;
    return (cur);
}

node_t *rbtree_max(const rbtree *t)
{
    node_t *cur = NULL;

    cur = t->root;
    while (cur->right != t->nil)
        cur = cur->right;
    return (cur);
}

// del
static void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
    if (u->parent == t->nil)
        t->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else // (u == u->parent->right)
        u->parent->right = v;
    v->parent = u->parent;
}

static void rbtree_erase_fixup(rbtree *t, node_t *x)
{
    node_t *w = NULL;

    while (x != t->root && x->color == RBTREE_BLACK)
    {
        // case 1-4: x is left child
        if (x == x->parent->left)
        {
            w = x->parent->right;
            // case 1: x's brother w is red
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                rbtree_rotate_left(t, x->parent);
                w = x->parent->right;
            }
            // case 2: x's brother w is black, w's l/r children are black
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
            {
                w->color = RBTREE_RED;
                x = x->parent;
            }
            // case 3: x's brother w is black, w's left child is red, w's right child is black
            else // (w->left->color == RBTREE_RED && w->right->color == RBTREE_BLACK)
            {
                if (w->right->color == RBTREE_BLACK)
                {
                    w->left->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    rbtree_rotate_right(t, w);
                    w = x->parent->right;
                }
                // case 4: x's brother w is black, w's right child is red
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->right->color = RBTREE_BLACK;
                rbtree_rotate_left(t, x->parent);
                x = t->root;
            }
        }
        // case 5-8: x is right child
        else // (x == x->parent->right)
        {
            w = x->parent->left;
            // case 5: x's brother w is red
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                rbtree_rotate_right(t, x->parent);
                w = x->parent->left;
            }
            // case 6: x's brother w is black, w's l/r children are black
            if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK)
            {
                w->color = RBTREE_RED;
                x = x->parent;
            }
            // case 7: x's brother w is black, w's left child is red, w's left child is black
            else // (w->right->color == RBTREE_RED && w->left->color == RBTREE_BLACK)
            {
                if (w->left->color == RBTREE_BLACK)
                {
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    rbtree_rotate_left(t, w);
                    w = x->parent->left;
                }
                // case 8: x's brother w is black, w's right child is red
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left->color = RBTREE_BLACK;
                rbtree_rotate_right(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p)
{
    node_t *x = NULL;
    node_t *y = NULL;
    color_t y_org_color = 0;

    if (!p)
        return (0); // err: not exist node
    y = p;
    y_org_color = y->color;
    if (p->left == t->nil)
    {
        x = p->right;
        rbtree_transplant(t, p, p->right);
    }
    else if (p->right == t->nil)
    {
        x = p->left;
        rbtree_transplant(t, p, p->left);
    }
    else // (p->left != t->nil && p->right != t->nil)
    {
        // successor: y
        y = p->right;
        while (y->left != t->nil)
            y = y->left;
        y_org_color = y->color;
        x = y->right;
        if (y->parent == p)
            x->parent = y;
        else // (y->parent != p)
        {
            rbtree_transplant(t, y, y->right);
            y->right = p->right;
            y->right->parent = y;
        }
        rbtree_transplant(t, p, y);
        y->left = p->left;
        y->left->parent = y;
        y->color = p->color;
    }
    if (y_org_color == RBTREE_BLACK)
        rbtree_erase_fixup(t, x);
    free(p);
    p = NULL;
    return (1);
}

// inorder traverse to array
static void rbtree_inorder_traverse(const rbtree *t, node_t *cur, key_t *arr, const size_t n, size_t *cnt)
{
    if (cur == t->nil)
        return ;
    if (cur->left != t->nil)
        rbtree_inorder_traverse(t, cur->left, arr, n, cnt);
    if (*cnt == n)
        return ;
    if (sizeof(arr[*cnt]) != sizeof(cur->key))
        return ;
    arr[(*cnt)++] = cur->key;
    if (cur->right != t->nil)
        rbtree_inorder_traverse(t, cur->right, arr, n, cnt);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
    size_t cnt = 0;

    if (!t || t->root == t->nil || !arr || n <= 0)
        return (0); // err: not exist tree or array or neg/zero n
    rbtree_inorder_traverse(t, t->root, arr, n, &cnt);
    if (cnt != n)
        return (0); // err: cnt != n
    return (1);
}
