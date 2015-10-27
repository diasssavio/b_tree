#include <stdio.h>
#include "btree.h"

int main(){
	b_tree * tree = init_tree();
	int num = 0, from;
	printf("Digite um numero para adicionar. 0 para imprimir. -9 para remover e -1 para sair\n");
	while(num != -1){
		scanf("%i", &num);
		if(num == -9){
		  scanf("%d", &from);
		  tree = remove_key(tree, from);
		}
		else if(num == -1){
		  printf("\n");
		  print_tree(tree, 0);
		  free_tree(tree);
		  break;
		}
		else if(!num){
		  printf("\n");
		  print_tree(tree, 0);
		}
		else tree = insert_key(tree, num);
	}
	return 0;
}