#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#define HEAP_SIZE 2000   //This is our heap block on which we will be managing malloc and free
#define N 3
#define SIZE 8

typedef struct meta_data_tag 
{
    size_t size;                    //size of the block
    struct meta_data_tag *next;     //pointer to next free block
}meta_data;
meta_data* free_list;               //creating a free_list linked list globally
void* initialize()
{
    meta_data* heap;                //This is whole heap block
    heap=malloc(HEAP_SIZE);         //malloc returns a void*
    heap->size=HEAP_SIZE-(sizeof(meta_data));   //The size of available block is HEAP_SIZE-metadata size
    heap->next=NULL;
    free_list=heap;                 //at first whole block is free
    return heap;
}
void split(meta_data* prev,meta_data* ptr,size_t req_size)    //Splitting the block and allocating only needed size
{
     meta_data *new=(meta_data *)((void *)ptr+sizeof(meta_data)+req_size);
     new->size=(ptr->size)-req_size-sizeof(meta_data);
     if(prev!=NULL)
     {
          new->next=prev->next;
          prev->next=new;        //new is added into free_list
     }
     else
     {
          new->next=free_list->next;
          free_list=new;          //if the block is allocated at the start of heap
     }
     //new->next=prev->next;
     ptr->size=req_size;         //as we are splitting the block size is req size only
}
       
void* Malloc(size_t req_size)
{
    meta_data *ptr=free_list;
    meta_data *prev,*result;
    prev=NULL;
    if(ptr==NULL)
    {
          printf("Memory is full\n");
          result=NULL;
    }
    else{
    while(ptr && (ptr->size)<req_size)  //Using First-fit
    {
          prev=ptr;
          ptr=ptr->next;
    }
    if(ptr==NULL)     //If it goes through all the available blocks and become null
    {
          printf("Memory is not full but no sufficient contiguous block of the size %ld\n",req_size);
          result=NULL;
    }
    else
    {
    if(((ptr->size)-req_size)>sizeof(meta_data))  //if we have have enough space to fit meta_data and some more then split
    {
          split(prev,ptr,req_size);
          printf("Fitting block is allocated with a split\n");
    }
    else
    {                                                //if the exact before block is not free
          if(prev!=NULL)                        
          prev->next=ptr->next;
          else
          free_list=free_list->next;
    }
    result=(void *)((char *)ptr + sizeof(meta_data));   //malloc must return the allocated space not the meta_data of block
    printf("The space is successfully allocated\n");
    }
    }
    return result;
}
void merge(meta_data *ptr1,meta_data *ptr2)
{
     ptr1->size+=(ptr2->size)+sizeof(meta_data);   //ptr1 is previous to ptr2 and both are free then merge
     printf("Merged size is %ld\n",(ptr1->size));
}
void Free(void *ptr) {
    if(ptr!=NULL)                        //if we try to free null
    {
    meta_data *nptr = ptr;               //ptr points to space it should point to meta_data to carry operations
    --nptr;
    meta_data *qptr = free_list;
    meta_data *prev = NULL;

    if (qptr != NULL) {                  //i.e, if the free_list is not empty
        while ((qptr) && (qptr) < nptr) {  
            prev = qptr;
            qptr = qptr->next;
        }
        // Merge with the next block if it's free
        if ((meta_data *)((char *)nptr + sizeof(meta_data) + nptr->size) == qptr) {
            merge(nptr,qptr);
            nptr->next = (qptr->next);
            //Merge with the previous block if it is free
            if (prev != NULL && (meta_data *)((char *)prev + sizeof(meta_data) + prev->size) == nptr) {
            merge(prev, nptr);
            prev->next=nptr->next;
           }
           else if(prev!=NULL)  //if it is not free
           {
               prev->next=nptr;
           } 
           else                   //if prev is null
           {
                free_list=nptr;
           }
        }
        else if (prev != NULL && (meta_data *)((char *)prev + sizeof(meta_data) + prev->size) == nptr) {
            merge(prev, nptr);             //next block is not free but previous block is free
            prev->next=qptr;
        } 
        else                                //if both are not free
        {
            nptr->next=qptr;
            if(prev!=NULL)
            {
                prev->next=nptr;
            }
            else
            {
            free_list=nptr;
            }
        }
     }
     else {
        free_list = nptr;
        free_list->next = NULL;
    }

    printf("The ptr is successfully freed\n");
   }
   else
   {
     printf("The given space is not allocated\n");
   }	
}
void Displayfree()
{
     meta_data *ptr=free_list;
     printf("The available free blocks are:\n");
     printf("ADDRESS\t\tSIZE\n");
     while(ptr)
     {
           printf("%p\t\t%ld\n",ptr,ptr->size);
           ptr=ptr->next;
     }
}

typedef struct node
{
    int data;
    bool mark;//for marking the nodes to use mark and sweep method
    int refCount;//reference count method
    struct node *next[N];
}Node;

Node *Array[SIZE];

//This function sets edges between nodes
void SetEdge(int source,int dest1,int dest2,int dest3)
{
    if(source >=0 && source < SIZE)
    {
         if(dest1 != -1)
         {
             Array[source]->next[0] = Array[dest1];
             Array[dest1]->refCount += 1;
         }
         if(dest2 != -1)
         {
             Array[source]->next[1] = Array[dest2];
             Array[dest2]->refCount += 1;
         }
         if(dest3 != -1)
         {
             Array[source]->next[2] = Array[dest3];
             Array[dest3]->refCount += 1;
         }
    }
}

int isEdgePresent(Node *root,int source,int dest)
{
     int i =0;
     if(root == NULL)
     {
         return 0;
     }
     
     if(root->data == source)
     {
          while(i<N)
          {
              if(root->next[i]!=NULL)
              {
                   if(root->next[i]->data == dest)
                   {
                        return 1;
                   }
              }
              i++;
          }
      }
      
      if(isEdgePresent(root->next[0],source,dest))
      {
           return 1;
      }
      
      if(isEdgePresent(root->next[1],source,dest))
      {
           return 1;
      }
      
      if(isEdgePresent(root->next[2],source,dest))
      {
           return 1;
      }
      
      return 0;
      
}
      
int Storehead(Node *root,int adjm[][SIZE+1],int is_initial_call)
{
    static int j,k = 1;
    if(is_initial_call == 1)
    {
         k = 1;
    }
    if(root!=NULL)
    {
        //printf("root->data:%d\n",root->data);
        adjm[0][k] = root->data;
	adjm[k][0] = root->data;
	k++;
    j = Storehead(root->next[0],adjm,++is_initial_call);
    j = Storehead(root->next[1],adjm,++is_initial_call);
    j = Storehead(root->next[2],adjm,++is_initial_call);
    }
    return k;
}

// it lists all the nodes that are connected for a particular vertex
void adjacency_list()
{
	int i=0;
	for(i=0;i<8;i++)
	{
		if(Array[i]!=NULL)
		{
			printf("Value=%d: ->",Array[i]->data);
			if(Array[i]->next[0]!=NULL)
			{
				printf("%d ->",Array[i]->next[0]->data);
			}
			if(Array[i]->next[1]!=NULL)
			{
				printf("%d ->",Array[i]->next[1]->data);
			}
			if(Array[i]->next[2]!=NULL)
			{

				printf("%d ->",Array[i]->next[2]->data);
			}
			printf("NULL\n");
		}
	}
}

void adjacency_Matrix(Node *root)
{
   if(root!=NULL)
   {
	int adjm[SIZE+1][SIZE+1];
	int i,j,k,numNodes;
	
	for(i=0;i<SIZE+1;i++)		//initialising all indices values to 0
	{
		for(j=0;j<SIZE+1;j++)
		{
			adjm[i][j]=0;
		}	
	}
	
	
	k = 1;
	numNodes = Storehead(root,adjm,1);
	for( i = 1 ; i<numNodes ; i++)
	{
	   for( j = 1 ; j<numNodes ; j++ )
	   {
	         adjm[i][j] = isEdgePresent(root,adjm[i][0],adjm[0][j]);
	   }
	}
	
	for(i=0;i<numNodes;i++)
	{
		for(j=0;j<numNodes;j++)
		{
			printf("%d ",adjm[i][j]);		//printing the adjacency matrix
		}
		printf("\n");
	} 
    }
}

void adjacency_Matrix_All()
{
	int adm[SIZE+1][SIZE+1];
	int i,j,k,numNodes = 0;
	
	for(i=0;i<SIZE+1;i++)		//initialising all indices values to 0
	{
		for(j=0;j<SIZE+1;j++)
		{
			adm[i][j]=0;
		}	
	}
	
	for(i=1;i<SIZE+1;i++)		
	{
	   if(Array[i-1]!=NULL)
	   {
	      //printf("Array data :%d\n",Array[i-1]->data);
	      adm[i][0]	= Array[i-1]->data;
	      adm[0][i] = Array[i-1]->data;
	      numNodes++;
	   }
	   else
	   {
	       adm[i][0]= -1;
	      adm[0][i] = -1;
	   }
	}
	//k = 0;
	for(i=1;i<SIZE+1;i++)
	{
	        //printf("For 1\n");
		for(j=1;j<SIZE+1;j++)
		{
		
		//printf("For 2\n");	
		if(Array[j-1]!=NULL && Array[i-1]!=NULL)
		{
			//printf("Into if\n");
			if(Array[i-1]->next[0]==Array[j-1])
			{
				//printf("Into if 1\n");
					adm[i][j]=1;
				
			}
			else if(Array[i-1]->next[1]==Array[j-1])
			{
				//printf("Into if 2\n");
					adm[i][j]=1;
				
			}
			else if(Array[i-1]->next[2]==Array[j-1])
			{
				//printf("Into if 3\n");
					adm[i][j]=1;
				
			}
		}
		
		}
	}
	
	
	for(i=0;i<SIZE+1;i++)
	{
	    if(adm[i][0]!=-1)
	    {
		for(j=0;j<SIZE+1;j++)
		{
		      if(adm[0][j]!=-1)
			printf("%d ",adm[i][j]);		//printing the adjacency matrix
		}
		printf("\n");
             }
             
	}
}

void referenceCounting()
{
    int j=0;
    while(j<SIZE)
    {
       if(Array[j]!=NULL)
       {
        if(Array[j]->refCount==0)
        {
            for(int i=0;i<N;i++)
            {
                if(Array[j]->next[i] != NULL)
                    Array[j]->next[i]->refCount -=1;
                    
            }
            printf("Garbage Node : %d\n",Array[j]->data);
            printf("Freed Size :%ld\n" , sizeof(Node));
            Free(Array[j]);
            Array[j]=NULL;
        }
        j++;
        }
    }
}

void mark_method(Node* root)
{
	
	if(root!=NULL)
	{
		root->mark=true;
	}
	if(root==NULL)
	{
		return;
	}
	mark_method(root->next[0]);
	mark_method(root->next[1]);
	mark_method(root->next[2]);
}

// frees the space if mark bit is false i.e. zero
void sweep_method()
{
	int i;
	for(i=0;i<SIZE;i++)
	{
		if(Array[i]->mark==false)
		{
		     for(int j=0;j<N;j++)
		     {
			if(Array[i]->next[j]!=NULL)
			{
				Array[i]->next[j]->refCount-=1;
			}
		      }
			printf("Garbage Node: %d\n",Array[i]->data);
			printf("freed Size :%ld\n",sizeof(Node));
			Free(Array[i]);
			Array[i]=NULL;
		}
	}
}

int main()
{
        meta_data *heap;
        heap=initialize();
        int val[]={1,2,3,5,7,8,9,10};
	int i;
	for(i=0;i<SIZE;i++)
	{
		Node* new_node =(Node*)Malloc(sizeof(Node));
		new_node->data=val[i];
		new_node->next[0]=NULL;
		new_node->next[1]=NULL;
		new_node->next[2]=NULL;
		new_node->refCount=0;
		new_node->mark=false;		
		Array[i]=new_node;
	}
	
	Node*root_1=Array[3];

	Array[3]->refCount+=1;
	Node*root_2=Array[0];
	Array[0]->refCount+=1;
	
	SetEdge(0,1,6,7);
	SetEdge(2,5,7,-1);
	SetEdge(3,0,-1,-1);
	SetEdge(4,0,5,-1);
	SetEdge(5,6,-1,-1);
	
	 //Displaying Adjacency Matrix of the nodes	
	printf("\n\nAdjacency matrix of root_1:\n");
	adjacency_Matrix(root_1);
	
	printf("\n\nAdjacency matrix of root_2:\n");
	adjacency_Matrix(root_2);
	
	printf("Adjacency list:\n");
	adjacency_list();
	adjacency_Matrix_All();
	int choice;
	printf("Do you want to perform Reference counting (input 1) or Mark and sweep (input 2)\n");
	scanf("%d",&choice);
	switch(choice)
	{
	 case 1:
	     printf("Reference Counting :\n");
	     referenceCounting();
	     printf("Adjacency matrix after reference counting:\n");
	     adjacency_Matrix_All();
	     break;
	 case 2:
	     printf("Mark and Sweep method\n");
	     mark_method(root_1);
	     mark_method(root_2);
	     sweep_method();
	     adjacency_Matrix_All();
	     break;
	 default:
            printf("Invalid option\n");
            // Handle invalid option
            break;
        }
	
	free(heap);
	
	return 0;
}




