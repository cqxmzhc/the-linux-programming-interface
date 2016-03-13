#include <stdio.h>
#include <unistd.h>

#define META_SIZE 24

/*实现简单的malloc,free函数*/
/*思路:
 * 1.堆内存可以通过sbrk系统调用改变program break的值来分配和释放
 * 2.和glibc的malloc函数实现一样,不应该每次分配内存时都调用sbrk来获取指定大小的内存空间.
 * 可以指定一个阈值,每次分配的内存大小是这个值的整数倍,然后再返回指定大小的内存给调用者
 * 3.对于内存的管理,需要做到以下几点
 *  一.如果请求的内存不足,则需要分配新的内存
 *  二.如果请求的内存足够,则从当前空闲的内存中返回指定大小的内存
 *  三.回收不使用的内存
*/

//通过一个单向链表结构来管理分配的内存
struct my_block {
    int free;                //是否空闲
    size_t size;             //内存大小
    struct my_block *next;   //下一个内存块指针
    char data[1];            //在对过大的空间分块时用做分界指针,也作为实际可使用空间的起始指针
};

typedef struct my_block *t_block;

//头指针
t_block pHead = NULL;

//内存分配函数
void *extend_mem(t_block prior, size_t size) {
    t_block block = (t_block)sbrk(size + META_SIZE);
    if (block == (void *) -1)
        return NULL;
    block->free = 0;
    block->size = size;
    block->next = NULL;
    if (prior)
        prior->next = block;
    return block;
}

//内存切分函数
void *split_mem (t_block cur_block, size_t size) {
    t_block new_block;
    new_block = (t_block)(cur_block->data + size);
    new_block->size = cur_block->size - META_SIZE - size;
    new_block->next = cur_block->next;
    new_block->free = 0;
    cur_block->next = new_block;
    return cur_block;
}

void *my_malloc(size_t size) {
    // 循环查看链表,是否有空闲空间可分配
    t_block p = pHead;
    t_block b;
    t_block prior;
    if (p) {
        while (p != NULL && !(p->next->free && p->next->size >= size)) {
            prior = p;
            p = p->next;
        }
        //没有空闲空间,进行分配
        if (!p) {
            b = (t_block)extend_mem(p, size);
        }
        //对内存进行切分,返回指定大小
        else if(p->size - size > META_SIZE) {
            b = (t_block)split_mem(p, size);
        }
        else
            b = p;
    }
    else {
        b = (t_block) extend_mem(NULL, size);
        pHead = b;
    }

    if (b) {
        printf("%p\n", b->data);
        return b->data;
    }
    else {
        return NULL;
    }

}

int main () {
    t_block pblock = (t_block)my_malloc(100);
    printf("%lu\n", pblock->size);
    return 0;
}
