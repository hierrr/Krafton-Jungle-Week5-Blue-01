#include "rbtree.h"

#include <stdlib.h>
#include <stddef.h>

rbtree *new_rbtree(void)
{
    rbtree *tree = (rbtree *)calloc(1, sizeof(rbtree));

    tree->nil = (node_t *)calloc(1, sizeof(node_t));
    tree->nil->color = RBTREE_BLACK;

    tree->root = tree->nil;

    return tree;
}

void delete_rbtree(rbtree *tree) 
{
    delete_node_recursive(tree, tree->root);

    free(tree->nil);
    tree->nil = NULL;

    free(tree);
    tree = NULL;
}

void delete_node_recursive(rbtree *tree, node_t *node) 
{
    if (node == tree->nil) { return; }

    delete_node_recursive(tree, node->left);
    delete_node_recursive(tree, node->right);

    free(node);
    node = NULL;
}

node_t *rbtree_insert(rbtree *tree, const key_t key) 
{
    node_t *z = (node_t *)calloc(1, sizeof(node_t));
    node_t *x = tree->root;
    node_t *y = tree->nil;
    z->key = key;

    while (x != tree->nil)
    {
        y = x;
        x = (z->key < y->key) ? x->left : x->right;
    }

    z->parent = y;
    if (y == tree->nil) { tree->root = z; }
    else
    {
        if (z->key < y->key) { y->left = z; }
        else { y->right = z; }
    }

    z->left = tree->nil;
    z->right = tree->nil;
    z->color = RBTREE_RED;

    rb_insert_fixup(tree, z);
    return z;
}

void rb_insert_fixup(rbtree *tree, node_t *z)
{
    node_t *y;

    while (z->parent->color == RBTREE_RED)
    {
        if (z->parent == z->parent->parent->left)
        {
            y = z->parent->parent->right;

            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else 
            {
                if (z == z->parent->right)
                {
                    z = z->parent;
                    left_rotate(tree, z);
                }
                
                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                right_rotate(tree, z->parent->parent);
            }
        }
        else
        {
            y = z->parent->parent->left;
            
            if (y->color == RBTREE_RED)
            {
                z->parent->color = RBTREE_BLACK;
                y->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                z = z->parent->parent;
            }
            else 
            {
                if (z == z->parent->left)
                {
                    z = z->parent;
                    right_rotate(tree, z);
                }

                z->parent->color = RBTREE_BLACK;
                z->parent->parent->color = RBTREE_RED;
                left_rotate(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = RBTREE_BLACK;
}

void left_rotate(rbtree *tree, node_t *x)
{
    node_t *y = x->right;
    x->right = y->left;
    
    if (y->left != tree->nil) { y->left->parent = x; }

    y->parent = x->parent;

    if (x->parent == tree->nil) { tree->root = y; }
    else if (x == x->parent->left) { x->parent->left = y; }
    else { x->parent->right = y; }

    y->left = x;
    x->parent = y;
}

void right_rotate(rbtree *tree, node_t *y)
{
    node_t *x = y->left;
    y->left = x->right;

    if (x->right != tree->nil) { x->right->parent = y; }

    x->parent = y->parent;

    if (y->parent == tree->nil) { tree->root = x; }
    else if (y == y->parent->left) { y->parent->left = x; }
    else { y->parent->right = x; }

    x->right = y;
    y->parent = x;
}

node_t *rbtree_find(const rbtree *tree, const key_t key) 
{
    node_t *cur = tree->root;

    while (cur != tree->nil && cur->key != key)
    {
        cur = (cur->key > key) ? cur->left : cur->right;
    }

    if (cur->key == key) { return cur; }
    
    return NULL;
}

node_t *rbtree_min(const rbtree *tree) 
{
    node_t *cur = tree->root;

    while (cur->left != tree->nil) { cur = cur->left; }
    return cur;
}

node_t *rbtree_max(const rbtree *tree) 
{
    node_t *cur = tree->root;

    while (cur->right != tree->nil) { cur = cur->right; }
    return cur;
}

int rbtree_erase(rbtree *tree, node_t *p) 
{
    if (p == NULL) { return 0; }
    
    node_t *y = p;
    node_t *x; 
    color_t y_original_color = p->color;

    if (p->left == tree->nil)
    {
        x = p->right;
        rb_transplant(tree, p, p->right);
    }

    else if (p->right == tree->nil)
    {
        x = p->left;
        rb_transplant(tree, p, p->left);
    }

    else
    {
        y = p->right;
        while (y->left != tree->nil) { y = y->left; }

        y_original_color = y->color;
        x = y->right;

        if (y->parent == p) { x->parent = y; }
        else
        {
            rb_transplant(tree, y, y->right);

            y->right = p->right;
            p->right->parent = y;
        }

        rb_transplant(tree,p,y);

        y->left = p->left;
        p->left->parent = y;
        y->color = p->color;
    }

    if (y_original_color == RBTREE_BLACK) { rb_erase_fixup(tree, x); }

    free(p);
    p = NULL;

    return 0;
}

void rb_transplant(rbtree *tree, node_t *u, node_t *v)
{
    if (u->parent == tree->nil) { tree->root = v; }
    else if (u == u->parent->left) { u->parent->left = v; }
    else { u->parent->right = v; }

    v->parent = u->parent;
}

void rb_erase_fixup(rbtree *tree, node_t *x) 
{
    node_t *w;
    
    while (x != tree->root && x->color == RBTREE_BLACK)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                
                x->parent->color = RBTREE_RED;
                
                left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
            {
                
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else
            {
                if (w->right->color == RBTREE_BLACK)
                {
                    w->left->color = RBTREE_BLACK;
                    
                    w->color = RBTREE_RED;
                    
                    right_rotate(tree, w);
                    w = x->parent->right;
                }
                
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->right->color = RBTREE_BLACK;
                left_rotate(tree, x->parent);
                x = tree->root;
            }
        }
        
        else
        {
            w = x->parent->left;
            if (w->color == RBTREE_RED)
            {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
            {
                w->color = RBTREE_RED;
                x = x->parent;
            }
            else
            {
                if (w->left->color == RBTREE_BLACK)
                {
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotate(tree, w);
                    w = x->parent->left;
                }

                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left->color = RBTREE_BLACK;

                right_rotate(tree, x->parent);
                x = tree->root;
            }
        }
    }

    x->color = RBTREE_BLACK;
}

int inorder(const rbtree *tree, node_t *p, key_t *keys, int i)
{
    if (p == tree->nil) { return i; }

    i = inorder(tree, p->left, keys, i);
    keys[i++] = p->key;
    i = inorder(tree, p->right, keys, i);

    return i;
}

int rbtree_to_array(const rbtree *tree, key_t *keys, const size_t n) 
{
    inorder(tree, tree->root, keys, 0);
    
    return 0;
}
