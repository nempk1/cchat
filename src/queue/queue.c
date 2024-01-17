#include "queue.h"

int squeue_init(squeue **ref)
{

	if((*ref) == NULL) { 
		(*ref) = (squeue *)malloc(sizeof(squeue));
		if((*ref) == NULL) 
			return ENOMEM;
	}
	(*ref)->head = NULL;
	(*ref)->tail = NULL;
	
	return pthread_mutex_init(&((*ref)->mutex), NULL);
}

int squeue_clear_all(squeue **ref)
{
       	squeue_node *traverse;

	if((*ref) == NULL)
		return 1;

	pthread_mutex_lock(&(*ref)->mutex);
	while((*ref)->head != NULL) {
		traverse = (*ref)->head;
		(*ref)->head = (*ref)->head->next;
		free(traverse->data);
		free(traverse);
	}
	(*ref)->tail = NULL;
	pthread_mutex_unlock(&(*ref)->mutex);

	return 0;
}

int squeue_clear_nodes(squeue **ref)
{
	squeue_node *traverse;

	if((*ref) == NULL)
		return 1;

       	pthread_mutex_lock(&(*ref)->mutex);
	while((*ref)->head != NULL) {
		traverse = (*ref)->head;
		(*ref)->head = (*ref)->head->next;
		free(traverse);
	}
	(*ref)->tail = NULL;
	pthread_mutex_unlock(&(*ref)->mutex);

	return 0;
}

int squeue_destroy(squeue **ref)
{
	if((*ref) == NULL)
		return 1;

	pthread_mutex_lock(&(*ref)->mutex);
	if((*ref)->head == NULL) {
		pthread_mutex_unlock(&(*ref)->mutex);
		pthread_mutex_destroy(&(*ref)->mutex);	
		free(*ref);
		(*ref) = NULL;
		return 0;
	}
	pthread_mutex_unlock(&(*ref)->mutex);

	return 0;
}

int squeue_isempty(squeue *ref)
{
	if(ref == NULL)
		return 2;
	
	int result = 1;

	pthread_mutex_lock(&ref->mutex);
	
	if(ref->head == NULL) 
		result = 0;
	
	pthread_mutex_unlock(&ref->mutex);
	return result;	
}

squeue_node* squeue_create_node(void *value)
{
	squeue_node *result = (squeue_node*)malloc(sizeof(squeue_node));
	if(result == NULL)
		return NULL;

	result->next = NULL;
	result->data = value;
	return result;
}


int squeue_enqueue_data(squeue *ref, void *data_arg)
{
	if(ref == NULL)
		return 1;

	squeue_node *data_node = squeue_create_node(data_arg);
	if(data_node == NULL)
		return 2;
		
	pthread_mutex_lock(&ref->mutex);
	if(ref->head == NULL) {
		ref->tail = ref->head = data_node;
	} else {
		ref->tail->next= data_node;
		ref->tail = data_node;
	}
	pthread_mutex_unlock(&ref->mutex);
	return 0;
}

int squeue_enqueue_node(squeue *ref, squeue_node *node)
{
	if(ref == NULL)
		return 1;

	pthread_mutex_lock(&ref->mutex);
	if(ref->head == NULL) { 
		ref->tail = ref->head = node;
	} else {
		ref->tail->next= node;
		ref->tail = node;
	}
	pthread_mutex_unlock(&ref->mutex);
	return 0;
}

int squeue_enqueue_cpy(squeue *ref, const void *data_arg, size_t data_size)
{
	void *data_cpy = (void*)(malloc(data_size));
	if(data_cpy == NULL)
		return ENOMEM;

	memcpy(data_cpy, data_arg, data_size);
	squeue_node *data_node = squeue_create_node(data_cpy);
	if(data_node == NULL) {
		free(data_cpy);
		return 1;
	}

	pthread_mutex_lock(&ref->mutex);
	if(ref->head == NULL) {
		ref->tail = ref->head = data_node;
	} else {
		ref->tail->next= data_node;
		ref->tail = data_node;
	}
	pthread_mutex_unlock(&ref->mutex);
	
	return 0;
}

void* squeue_dequeue_data(squeue *ref)
{
	squeue_node *tmp; 

	if(squeue_dequeue_node(ref, &tmp)) 
		return NULL;		
	
	void *data_ref = tmp->data;
	free(tmp);
	return data_ref;
}

int squeue_dequeue_node(squeue *ref, squeue_node **result)
{
	if(ref == NULL) {
		return 1;
	}

	pthread_mutex_lock(&ref->mutex);

	if(ref->head == NULL) {
		pthread_mutex_unlock(&ref->mutex);
		return 2;
	}
	
	squeue_node *tmp = ref->head;	
	ref->head = ref->head->next;

	pthread_mutex_unlock(&ref->mutex);
	*result = tmp;
	return 0;
}
