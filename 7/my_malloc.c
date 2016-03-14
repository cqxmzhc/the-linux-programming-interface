#include <stdio.h>
#include <unistd.h>

#define META_SIZE 40

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

/*free函数:
 * 1.释放内存前首先需要检查地址的合法性:要释放的内存地址必须是我们的malloc函数分配出去的.
 * 通过与链表的第一个元素地址以及program break地址比较来判断(在二者的地址区间内)
 * 2.释放内存时需要考虑碎片问题
 */

//通过一个双向链表结构来管理分配的内存
struct my_block {
    int free;                //是否空闲
    size_t size;             //内存大小
    struct my_block *prev;  //上一个内存块地址
    struct my_block *next;   //下一个内存块指针
    void *ptr;               //指向data,用于判断此block是我们自己的malloc函数分配的
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
    block->ptr = block->data;
    if (prior) {
        block->prev = prior;
        prior->next = block;
    }
    else
        block->prev = NULL;
    return block;
}

//内存切分函数
void *split_mem (t_block cur_block, size_t size) {
    t_block new_block;
    new_block = (t_block)(cur_block->data + size);
    new_block->size = cur_block->size - META_SIZE - size;
    new_block->next = cur_block->next;
    new_block->prev = cur_block;
    new_block->free = 1;
    new_block->ptr = new_block->data;
    cur_block->next = new_block;
    return cur_block;
}

//获取addr所在的block
t_block getBlock (void *addr) {
    char *tmp;
    tmp = addr;
    return (t_block)(tmp-META_SIZE);
}

//检查地址合法性
int valid_addr(void *addr) {
    if (pHead) {
        //地址是否在第一个block和program break区间内
        if (addr >= (void *)pHead && addr <= sbrk(0)) {
            return (t_block)addr == (t_block)(getBlock(addr)->ptr);
        }
        else
            return 0;
    }
    else
        return 0;
}

//合并相邻的空闲内存

t_block merge (t_block block) {
    block->next->free = 1;
    block->size += block->next->size + META_SIZE;
    block->next = block->next->next;
    if (block->next) {
        block->next->prev = block;
    }

    return block;
}

void *my_malloc(size_t size) {
    // 循环查看链表,是否有空闲空间可分配
    t_block p = pHead;
    t_block b;
    t_block prior;
    if (p) {
        while (p && !(p->free && p->size >= (size + META_SIZE))) {
            prior = p;
            p = p->next;
        }
        //没有空闲空间,进行分配
        if (!p) {
            b = (t_block)extend_mem(prior, size);
        }
        //对内存进行切分,返回指定大小
        else{
            b = (t_block)split_mem(p, size);
        }
    }
    else {
        b = (t_block) extend_mem(NULL, size);
        pHead = b;
    }

    if (b) {
        return b->data;
    }
    else {
        return NULL;
    }

}

void my_free(t_block freeBlock) {
    //检查地址合法性
    if (valid_addr(freeBlock)) {
        t_block tmpBlock = getBlock(freeBlock);
        tmpBlock->free = 1;
        if (tmpBlock->prev && tmpBlock->prev->free) {
            //合并相邻的空闲内存
            tmpBlock = merge(tmpBlock->prev);
        }
        if (tmpBlock->next && tmpBlock->next->free) {
           merge(tmpBlock);
        }
        // 判断当前block是否是末尾的block,如果是就调用sbrk回收
        if (!tmpBlock->next) {
            // 判断当前block是否是最后(唯一)一个block,若是则需要将头指针设为NULL;
           if (tmpBlock->prev)
               tmpBlock->prev->next = NULL;
           else
               pHead = NULL;

           brk(tmpBlock);
        }
    }
}

int main () {
    printf("program break is %p before my_alloc\n", sbrk(0));
    t_block pblock = (t_block)my_malloc(100);
    printf("program break is %p after my_alloc(100)\n", sbrk(0));
    t_block pblock1 = (t_block)my_malloc(100);
    printf("program break is %p after my_alloc(100) again\n", sbrk(0));
    my_free(pblock);
    printf("program break is %p after my_free\n", sbrk(0));
    my_free(pblock1);
    printf("program break is %p after my_free again\n", sbrk(0));
    return 0;
}
