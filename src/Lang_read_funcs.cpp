#include "Lang_read_funcs.h"

Node* Lang_read_expression(const char* filename, Var_list* func_name_list,
						   Buffer_data* expression_buffer, Func_data_list* func_list,
						   Lexeme_array** lexeme_arr) {

	*expression_buffer = Lang_read_file(filename);
	if(!expression_buffer->buffer || expression_buffer->buffer_size < 0)
		return NULL;

	*lexeme_arr = Lexeme_separator(expression_buffer->buffer, expression_buffer->buffer_size, func_name_list);
	if(!(*lexeme_arr))
		return NULL;

	func_list->size = func_name_list->free_var + 2; // + 2 will be needed for extra func check_alignment
	func_list->func_data = (Func_data*) calloc(func_list->size, sizeof(Func_data));
	if(!func_list->func_data) {

		DEBUG_PRINTF("ERROR: memory was not allocated\n");
		return NULL;
	}

	int64_t curr_lex = 0;
	return Get_Grammar(*lexeme_arr, &curr_lex, func_list);
}

Buffer_data Lang_read_file(const char* filename) {

	Buffer_data buffer_data = {};
	int64_t file_size = get_file_size(filename);

	if(file_size < 0)
		return buffer_data;

	buffer_data.buffer_size = file_size;

	FILE* input_file = fopen(filename, "rb");
	if(!input_file) {

		DEBUG_PRINTF("ERROR: file was not opened\n");
		return buffer_data;
	}

	char* file_buffer = (char*) calloc(file_size, sizeof(char));
	if(!file_buffer) {

		DEBUG_PRINTF("ERROR: memory was not allocated\n");
		return buffer_data;
	}

	fread(file_buffer, sizeof(char), file_size, input_file);
	if(ferror(input_file)) {

		DEBUG_PRINTF("ERROR: fread failed\n");
		return buffer_data;
	}

	buffer_data.buffer = file_buffer;
	fclose(input_file);
	return buffer_data;
}

bool Lexeme_array_dtor(Lexeme_array* lexeme_array) {

	if(!lexeme_array || !lexeme_array->lex_arr) {

		DEBUG_PRINTF("ERROR: null ptr\n");
		return false;
	}

	for(int64_t lex_index = 0; lex_index < lexeme_array->size; lex_index++) {

		Node_data* tmp_data = NULL;
		memcpy(&tmp_data, &lexeme_array->lex_arr[lex_index].root->node_data, sizeof(Node_data*));
		if(tmp_data->expression_type == VAR || tmp_data->expression_type == FUNCTION) {

			Variable_data* var_data = NULL;
			memcpy(&var_data, &tmp_data->value, sizeof(Variable_data*));
			free(var_data);
		}
	}

	free(lexeme_array->lex_arr);
	lexeme_array->size = 0;
	free(lexeme_array);
	return true;
}

Lexeme_array* Lexeme_separator(char* expr_buffer, int64_t expr_buffer_size, Var_list* func_name_list) {

	Lexeme_array* lexeme_array = (Lexeme_array*) calloc(1, sizeof(Lexeme_array));
	lexeme_array->lex_arr = (Lexeme_data*) calloc(expr_buffer_size + Mem_to_check, sizeof(Lexeme_data));
	size_t lexeme_array_pos = 0;

	if(!lexeme_array) {

		DEBUG_PRINTF("ERROR: memory was not allocated\n");
		return NULL;
	}

	for(int64_t curr_pos = 0, line = 1, col = 1; curr_pos < expr_buffer_size;) {

		if(!Lang_get_next_symbol(expr_buffer, expr_buffer_size,
								 &curr_pos, &line, &col))
			break;

		lexeme_array->lex_arr[lexeme_array_pos].line = line;
		lexeme_array->lex_arr[lexeme_array_pos].col = col;

		if(expr_buffer[curr_pos] == '\n') {

			line++;
			col = 1;
		}

		if(expr_buffer[curr_pos] == '\t') {

			col += 4;
		}

		if(!strncmp(expr_buffer + curr_pos, "log", sizeof("log") - 1) &&
				!isalpha(expr_buffer[curr_pos + sizeof("log") - 1])) {

			lexeme_array->lex_arr[lexeme_array_pos].root = _LOG(NULL, NULL);
			curr_pos += sizeof("log") - 1;
			col += sizeof("log") - 1;
			lexeme_array_pos++;
		}

		else if(isdigit(expr_buffer[curr_pos])) {

			double tmp_double = 0;
			int num_len = 0;
			sscanf(expr_buffer + curr_pos, "%lf%n", &tmp_double, &num_len);
			col += num_len;
			curr_pos += num_len;
			lexeme_array->lex_arr[lexeme_array_pos].root = _NUM(tmp_double);
			lexeme_array_pos++;
		}

		#define FUNC(func_name, func_full_name) \
			else if(!strncmp(expr_buffer + curr_pos, func_full_name, sizeof(func_full_name) - 1) && \
					!isalpha(expr_buffer[curr_pos + sizeof(func_full_name) - 1])) {\
			\
				lexeme_array->lex_arr[lexeme_array_pos].root = Lang_new_node(OP, &func_name, \
																	 sizeof(int64_t), NULL, NULL);\
				curr_pos += sizeof(func_full_name) - 1;\
				col += sizeof(func_full_name) - 1;\
			\
				lexeme_array_pos++;\
			}

		#define FUNC_BASIC(func_name, func_full_name) \
			else if(!strncmp(expr_buffer + curr_pos, func_full_name, sizeof(func_full_name) - 1)) {\
			\
				lexeme_array->lex_arr[lexeme_array_pos].root = Lang_new_node(OP, &func_name, \
																	 sizeof(int64_t), NULL, NULL);\
				curr_pos += sizeof(func_full_name) - 1;\
				col += sizeof(func_full_name) - 1;\
			\
				lexeme_array_pos++;\
			}

		#define KEY_WORD(func_name, func_full_name) \
			else if(!strncmp(expr_buffer + curr_pos, func_full_name, sizeof(func_full_name) - 1) && \
					!isalpha(expr_buffer[curr_pos + sizeof(func_full_name) - 1])) {\
			\
				lexeme_array->lex_arr[lexeme_array_pos].root = Lang_new_node(KEY_WORD, &func_name, \
																	 sizeof(int64_t), NULL, NULL);\
				curr_pos += sizeof(func_full_name) - 1;\
				col += sizeof(func_full_name) - 1;\
			\
				lexeme_array_pos++;\
			}

		#include "Key_words_code_gen.h"
		#include "Funcs_code_gen.h"
		#undef FUNC
		#undef FUNC_BASIC
		#undef KEY_WORD

		else {
			#define SPECIAL_SYMBOL(value, symbol)   case symbol: {\
														lexeme_array->lex_arr[lexeme_array_pos].root = \
														Lang_new_node(SPECIAL_SYMBOL, &value,\
																	sizeof(int64_t),\
																	NULL, NULL);\
														curr_pos++;\
														col++;\
														lexeme_array_pos++;\
														continue;\
													}

			switch(expr_buffer[curr_pos]) {

				#include "Special_symbols_code_gen.h"
			}


			if(expr_buffer[curr_pos] == 'e' && !isalpha(expr_buffer[curr_pos + 1])) {

				double tmp_double = M_E;
				curr_pos++;
				col++;
				lexeme_array->lex_arr[lexeme_array_pos].root = _NUM(tmp_double);
				lexeme_array_pos++;
			}

			else if(!strncmp(expr_buffer + curr_pos, "pi", 2) && !isalpha(expr_buffer[curr_pos + 2])) {

				double tmp_double = M_PI;
				curr_pos += 2;
				col += 2;
				lexeme_array->lex_arr[lexeme_array_pos].root = _NUM(tmp_double);
				lexeme_array_pos++;
			}

			else if(!strncmp(expr_buffer + curr_pos, "INF", 3) && !isalpha(expr_buffer[curr_pos + 3])) {

				double tmp_double = INFINITY;
				curr_pos += 3;
				col += 3;
				lexeme_array->lex_arr[lexeme_array_pos].root = _NUM(tmp_double);
				lexeme_array_pos++;
			}

			else if(!strncmp(expr_buffer + curr_pos, "NAN", 3) && !isalpha(expr_buffer[curr_pos + 3])) {

				double tmp_double = NAN;
				curr_pos += 3;
				col += 3;
				lexeme_array->lex_arr[lexeme_array_pos].root = _NUM(tmp_double);
				lexeme_array_pos++;
			}

			else if(isalpha(expr_buffer[curr_pos]) || expr_buffer[curr_pos] == '_'){

				uint64_t pos_copy = curr_pos;
				while(isalpha(expr_buffer[curr_pos]) || isdigit(expr_buffer[curr_pos]) || expr_buffer[curr_pos] == '_')
					curr_pos++;

				uint64_t var_len = curr_pos - pos_copy;
				Variable_data* vl_ptr = NULL;
				vl_ptr = Create_variable_data(expr_buffer + pos_copy, var_len, Var_list_init_value, line, col);

				if(expr_buffer[curr_pos] == '(') {

					int64_t index = Insert_var(func_name_list, expr_buffer + pos_copy, Var_list_init_value, var_len);
					if(index == Var_list_error_value) {

						free(lexeme_array->lex_arr);
						free(lexeme_array);
						return NULL;
					}

					lexeme_array->lex_arr[lexeme_array_pos].root = _FUNC(vl_ptr);
				}

				else {

					lexeme_array->lex_arr[lexeme_array_pos].root = _VAR(vl_ptr);
				}

				col += var_len;
				lexeme_array_pos++;
			}

			else {

				DEBUG_PRINTF("ERROR: unknown syntax option '%c', line %d, col %d\n", expr_buffer[curr_pos], line, col);
				free(lexeme_array->lex_arr);
				free(lexeme_array);
				return NULL;
			}
		}
	}

	#undef SPECIAL_SYMBOL

	lexeme_array->size = lexeme_array_pos;

	return lexeme_array;
}

Node* Get_Grammar(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Skip_lexemes(lexeme_array, curr_lex);
	Node_data* tmp_data = NULL;
	int offset = 0;

	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	if(tmp_data->value != ANGLE_BRACKET_OP || tmp_data->expression_type != SPECIAL_SYMBOL)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);

	Node* root = Get_user_function_def(lexeme_array, curr_lex, &offset, func_list);

	int64_t connector_index = *curr_lex - 1;
	Skip_lexemes(lexeme_array, curr_lex);

	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	while(tmp_data->value == ANGLE_BRACKET_OP &&
		  tmp_data->expression_type == SPECIAL_SYMBOL) {

		(*curr_lex)++;
		Node* connector = lexeme_array->lex_arr[connector_index].root;
		Skip_lexemes(lexeme_array, curr_lex);

		connector->left_node = root;

		connector->right_node = Get_user_function_def(lexeme_array, curr_lex, &offset, func_list);

		root = connector;
		connector_index = *curr_lex - 1;
		Skip_lexemes(lexeme_array, curr_lex);
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	}

	return root;
}

Node* Get_if(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset, Func_data_list* func_list) {

	int64_t copy_lex_index = *curr_lex;
	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);

	lexeme_array->lex_arr[copy_lex_index].root->left_node = Get_condition(lexeme_array, curr_lex, func_list); // does (*curr_lex)++;

	Skip_lexemes(lexeme_array, curr_lex);
	Node_data* tmp_data = NULL;

	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	Node* yes_node = NULL, * no_node = NULL;
	if(tmp_data->value == YES && tmp_data->expression_type == KEY_WORD) {

		(*curr_lex)++;
		Skip_lexemes(lexeme_array, curr_lex);
		(*offset)++;

		yes_node = Get_operation(lexeme_array, curr_lex, offset, func_list);

		(*offset)--;
	}

	else
		SYNTAX_ERROR(lexeme_array, curr_lex);

	int64_t curr_lex_copy = *curr_lex;
	Skip_lexemes(lexeme_array, &curr_lex_copy);
	memcpy(&tmp_data, &lexeme_array->lex_arr[curr_lex_copy].root->node_data, sizeof(Node_data*));

	if(tmp_data->value == NO && tmp_data->expression_type == KEY_WORD) {

		*curr_lex = curr_lex_copy + 1;
		Skip_lexemes(lexeme_array, curr_lex);
		(*offset)++;

		no_node = Get_operation(lexeme_array, curr_lex, offset, func_list);

		(*offset)--;
	}

	lexeme_array->lex_arr[copy_lex_index].root->right_node = _IF_BRANCHES(yes_node, no_node);

	return lexeme_array->lex_arr[copy_lex_index].root;
}

Node* Get_while(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset,
                Func_data_list* func_list) {

	int64_t copy_lex_index = *curr_lex;
	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);

	lexeme_array->lex_arr[copy_lex_index].root->left_node = Get_condition(lexeme_array, curr_lex, func_list);

	Skip_lexemes(lexeme_array, curr_lex);
	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->value != COLON || tmp_data->expression_type != SPECIAL_SYMBOL)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);
	(*offset)++;

	lexeme_array->lex_arr[copy_lex_index].root->right_node = Get_operation(lexeme_array, curr_lex, offset, func_list);

	(*offset)--;

	return lexeme_array->lex_arr[copy_lex_index].root;
}

Node* Get_condition(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node* left_node = NULL, * right_node = NULL, * root = NULL;

	left_node = Get_addition_priority(lexeme_array, curr_lex, func_list);

	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex-1].root->node_data, sizeof(Node_data*));
	Skip_lexemes(lexeme_array, curr_lex);

	Node_data* tmp_data1 = NULL, * tmp_data2 = NULL;
	int64_t old_pos = 0;
	if(lexeme_array->lex_arr[*curr_lex].root) {

		memcpy(&tmp_data1, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
		if(lexeme_array->lex_arr[*curr_lex + 1].root)
			memcpy(&tmp_data2, &lexeme_array->lex_arr[*curr_lex + 1].root->node_data, sizeof(Node_data*));

		free(lexeme_array->lex_arr[*curr_lex].root);
		old_pos = *curr_lex;

		lexeme_array->lex_arr[*curr_lex].root = Get_comparison(tmp_data1, tmp_data2, curr_lex);
	}

	Skip_lexemes(lexeme_array, curr_lex);
	lexeme_array->lex_arr[old_pos].root->left_node = left_node;

	lexeme_array->lex_arr[old_pos].root->right_node = Get_addition_priority(lexeme_array, curr_lex, func_list);

	return lexeme_array->lex_arr[old_pos].root;
}

Node* Get_comparison(Node_data* node_data1, Node_data* node_data2, int64_t* curr_lex) {

	if(node_data1->value == ANGLE_BRACKET_OP) {

		if(node_data2->value == ASSIGNMENT) {

			(*curr_lex) += 2;
			return Lang_new_node(CONDITION, &LESS_OR_EQUAL, sizeof(int64_t), NULL, NULL);
		}

		else {

			(*curr_lex)++;
			return Lang_new_node(CONDITION, &LESS, sizeof(int64_t), NULL, NULL);
		}
	}

	else if(node_data1->value == ANGLE_BRACKET_CL) {

		if(node_data2->value == ASSIGNMENT) {

			(*curr_lex) += 2;
			return Lang_new_node(CONDITION, &GREATER_OR_EQUAL, sizeof(int64_t), NULL, NULL);
		}

		else {

			(*curr_lex)++;
			return Lang_new_node(CONDITION, &GREATER, sizeof(int64_t), NULL, NULL);
		}
	}

	else if(node_data1->value == ASSIGNMENT) {

		(*curr_lex)++;
		return Lang_new_node(CONDITION, &EQUAL, sizeof(int64_t), NULL, NULL);
	}

	else if(node_data1->value == EX_MARK) {

		(*curr_lex) += 2;
		return Lang_new_node(CONDITION, &NOT_EQUAL, sizeof(int64_t), NULL, NULL);
	}

	return NULL;
}

Node* Get_user_function_def(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset,
							Func_data_list* func_list) {

	(*offset)++;
	Node_data* tmp_data = NULL, * bracket_node = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->expression_type != FUNCTION)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	memcpy(&func_list->func_data[func_list->free_element].function, &tmp_data->value,
		   sizeof(Variable_data*));
	Node* root = lexeme_array->lex_arr[*curr_lex].root;
	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);

	memcpy(&bracket_node, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	if(bracket_node->value != PARENTHESES_OP || bracket_node->expression_type != SPECIAL_SYMBOL)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	(*curr_lex)++;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->expression_type != SPECIAL_SYMBOL &&
	   tmp_data->value != PARENTHESES_CL) {

			func_list->func_data[func_list->free_element].func_memory = 0;
			if(!Var_list_ctor(&func_list->func_data[func_list->free_element].parameters, Var_list_init_size))
				return NULL;

			root->left_node = Get_def_parameters(lexeme_array, curr_lex, func_list);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
		if(tmp_data->expression_type != SPECIAL_SYMBOL || tmp_data->value != PARENTHESES_CL)
		   SYNTAX_ERROR(lexeme_array, curr_lex);
	}

	(*curr_lex)++;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->value != NEW_LINE || tmp_data->expression_type != SPECIAL_SYMBOL)
	   SYNTAX_ERROR(lexeme_array, curr_lex);

	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);

	if(!Var_list_ctor(&func_list->func_data[func_list->free_element].local_vars, Var_list_init_size))
		return NULL;

	root->right_node = Get_operation(lexeme_array, curr_lex, offset, func_list);

	Skip_lexemes(lexeme_array, curr_lex);
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->value != ANGLE_BRACKET_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	(*curr_lex)++;
	(*offset)--;
	func_list->free_element++;

	return root;
}

Node* Get_user_function(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node_data* tmp_data = NULL, * bracket_node = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->expression_type != FUNCTION)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	Node* root = lexeme_array->lex_arr[*curr_lex].root;

	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);

	memcpy(&bracket_node, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	if(bracket_node->value != PARENTHESES_OP || bracket_node->expression_type != SPECIAL_SYMBOL)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	(*curr_lex)++;

	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->expression_type != SPECIAL_SYMBOL &&
	   tmp_data->value != PARENTHESES_CL) {

		root->left_node = Get_parameters(lexeme_array, curr_lex, func_list);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

		if(tmp_data->expression_type != SPECIAL_SYMBOL ||
		   tmp_data->value != PARENTHESES_CL)
		   SYNTAX_ERROR(lexeme_array, curr_lex);
	}

	(*curr_lex)++;

	return root;
}

Node* Get_def_parameters(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node_data* tmp_data = NULL;
	Node* root = lexeme_array->lex_arr[*curr_lex].root;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	if(tmp_data->expression_type != VAR)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	Variable_data* var_data = NULL;
	int64_t index = 0;

	INSERT_PARAMETER

	(*curr_lex)++;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	while(tmp_data->value == COMMA && tmp_data->expression_type == SPECIAL_SYMBOL) {

		Node* connector = lexeme_array->lex_arr[*curr_lex].root;
		connector->right_node = root;
		(*curr_lex)++;
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

		if(tmp_data->expression_type != VAR)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		INSERT_PARAMETER

		connector->left_node = lexeme_array->lex_arr[*curr_lex].root;
		root = connector;
		(*curr_lex)++;
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	}

	return root;
}

Node* Get_parameters(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node_data* tmp_data = NULL;
	Node* root = Get_addition_priority(lexeme_array, curr_lex, func_list);
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	while(tmp_data->value == COMMA && tmp_data->expression_type == SPECIAL_SYMBOL) {

		Node* connector = lexeme_array->lex_arr[*curr_lex].root;

		connector->right_node = root;
		(*curr_lex)++;

		connector->left_node = Get_addition_priority(lexeme_array, curr_lex, func_list);

		root = connector;
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	}

	return root;
}

Node* Get_operation(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset,
                    Func_data_list* func_list) {

	Node* root = NULL, * left_node = NULL;
	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->value == IF && tmp_data->expression_type == KEY_WORD) {

		root = Get_if(lexeme_array, curr_lex, offset, func_list);
	}

	else if(tmp_data->value == WHILE && tmp_data->expression_type == KEY_WORD) {

		root = Get_while(lexeme_array, curr_lex, offset, func_list);
	}

	else if(tmp_data->expression_type == VAR) {

		INSERT_LOC_VAR

		root = Get_assignment(lexeme_array, curr_lex, func_list);
	}

	else if(tmp_data->expression_type == FUNCTION) {

		root = Get_user_function(lexeme_array, curr_lex, func_list);
	}

	else if(tmp_data->value == RETURN && tmp_data->expression_type == KEY_WORD) {

		int64_t curr_lex_copy = *curr_lex;
		(*curr_lex)++;

		lexeme_array->lex_arr[curr_lex_copy].root->right_node = Get_addition_priority(lexeme_array, curr_lex, func_list);

		root = lexeme_array->lex_arr[curr_lex_copy].root;
	}

	if(!root) {

		DEBUG_PRINTF("ERROR: root null ptr\n");
		return NULL;
	}

	while(!Exit_operator(lexeme_array, curr_lex, offset)) {

		Skip_lexemes(lexeme_array, curr_lex);
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
		Node* connector = lexeme_array->lex_arr[*curr_lex-1].root;
		connector->left_node = root;

		connector->right_node = Get_operation(lexeme_array, curr_lex, offset, func_list);

		root = connector;
	}

	return root;
}

Node* Get_assignment(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node* var_node = lexeme_array->lex_arr[*curr_lex].root;
	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);
	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->value != ASSIGNMENT || tmp_data->expression_type != SPECIAL_SYMBOL)
		SYNTAX_ERROR(lexeme_array, curr_lex);

	Node* assignment_node = lexeme_array->lex_arr[*curr_lex].root;
	(*curr_lex)++;
	Skip_lexemes(lexeme_array, curr_lex);

	assignment_node->right_node = Get_addition_priority(lexeme_array, curr_lex, func_list);
	assignment_node->left_node = var_node;

	return assignment_node;
}

bool Exit_operator(Lexeme_array* lexeme_array, int64_t* curr_lex, int* offset) {

	int64_t curr_lex_copy = *curr_lex;
	Node_data* tmp_data = NULL;
	int tabs_amount = 0;
	memcpy(&tmp_data, &lexeme_array->lex_arr[curr_lex_copy].root->node_data, sizeof(Node_data*));

	while(tmp_data->expression_type == SPECIAL_SYMBOL &&
		 (tmp_data->value == TAB || tmp_data->value == NEW_LINE)) {

		if(tmp_data->value == NEW_LINE)
			tabs_amount = 0;

		else
			tabs_amount++;

		(curr_lex_copy)++;
		if(curr_lex_copy < lexeme_array->size)
			memcpy(&tmp_data, &lexeme_array->lex_arr[curr_lex_copy].root->node_data, sizeof(Node_data*));

		else
			return true;
	}

	if(tabs_amount < *offset || tmp_data->expression_type == SPECIAL_SYMBOL &&
								tmp_data->value == ANGLE_BRACKET_CL) {

		return true;
	}

	return false;
}

void Skip_lexemes(Lexeme_array* lexeme_array, int64_t* curr_lex) {

	Node_data* tmp_data = NULL;
	if(*curr_lex < lexeme_array->size)
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	while(*curr_lex + 1 < lexeme_array->size && tmp_data->expression_type == SPECIAL_SYMBOL &&
		 (tmp_data->value == TAB || tmp_data->value == NEW_LINE)) {

		(*curr_lex)++;
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	}
}

Node* Get_addition_priority(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node* root = Get_multiplication_priority(lexeme_array, curr_lex, func_list);

	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	while(tmp_data->expression_type == OP &&
		 (tmp_data->value == ADD || tmp_data->value == SUB)) {

		Node* operation = lexeme_array->lex_arr[*curr_lex].root;
		operation->left_node = root;
		(*curr_lex)++;

		Node* right_node = Get_multiplication_priority(lexeme_array, curr_lex, func_list);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
		operation->right_node = right_node;

		root = operation;
	}

	return root;
}

Node* Get_multiplication_priority(Lexeme_array* lexeme_array, int64_t* curr_lex,
                                  Func_data_list* func_list) {

	Node* root = Get_power_priority(lexeme_array, curr_lex, func_list);

	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	while(tmp_data->expression_type == OP &&
		 (tmp_data->value == MUL || tmp_data->value == DIV)) {

		Node* operation = lexeme_array->lex_arr[*curr_lex].root;
		operation->left_node = root;
		(*curr_lex)++;

		Node* right_node = Get_power_priority(lexeme_array, curr_lex, func_list);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
		operation->right_node = right_node;

		root = operation;
	}

	return root;
}

Node* Get_power_priority(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node* root = Get_first_priority(lexeme_array, curr_lex, func_list);

	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	while(tmp_data->expression_type == OP && tmp_data->value == POW) {

		Node* operation = lexeme_array->lex_arr[*curr_lex].root;
		operation->left_node = root;
		(*curr_lex)++;

		Node* right_node = Get_first_priority(lexeme_array, curr_lex, func_list);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
		operation->right_node = right_node;

		root = operation;
	}

	return root;
}

Node* Get_first_priority(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->value == PARENTHESES_OP && tmp_data->expression_type == SPECIAL_SYMBOL) {

		(*curr_lex)++;

		Node* root = Get_addition_priority(lexeme_array, curr_lex, func_list);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
		if(tmp_data->value != PARENTHESES_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		(*curr_lex)++;

		return root;
	}

	else if(tmp_data->expression_type == NUM) {      // Get_Num

		(*curr_lex)++;
		return lexeme_array->lex_arr[*curr_lex - 1].root;
	}

	else if(tmp_data->expression_type == OP) { // Get_Func

		if(tmp_data->value == SUB) {

			Node_data* tmp_data1 = NULL;
			memcpy(&tmp_data1, &lexeme_array->lex_arr[*curr_lex-1].root->node_data, sizeof(Node_data*));

			if(tmp_data1->expression_type == CONDITION ||
			   tmp_data1->expression_type == KEY_WORD  ||
			   tmp_data1->expression_type == SPECIAL_SYMBOL &&
			  (tmp_data1->value == ASSIGNMENT || tmp_data1->value == PARENTHESES_OP)) {

				memcpy(&tmp_data1, &lexeme_array->lex_arr[*curr_lex+1].root->node_data, sizeof(Node_data*));

				if(tmp_data1->expression_type == NUM) {

					double tmp_double = 0;
					memcpy(&tmp_double, &tmp_data1->value, sizeof(double));
					tmp_double = -tmp_double;
					memcpy(&tmp_data1->value, &tmp_double, sizeof(double));
					*curr_lex += 2;
					return lexeme_array->lex_arr[*curr_lex-1].root;
				}

				else {

					tmp_data->value = MUL;
					double neg_1 = -1;
					int64_t curr_lex_copy = *curr_lex;
					lexeme_array->lex_arr[lexeme_array->size].root = _NUM(neg_1);

					lexeme_array->lex_arr[curr_lex_copy].root->left_node =
					lexeme_array->lex_arr[lexeme_array->size].root;
					lexeme_array->size++;
					(*curr_lex)++;

					lexeme_array->lex_arr[curr_lex_copy].root->right_node =
					Get_first_priority(lexeme_array, curr_lex, func_list);

					return lexeme_array->lex_arr[curr_lex_copy].root;
				}
			}
		}

		else
			return Get_function(lexeme_array, curr_lex, func_list);
	}

	else if(tmp_data->expression_type == FUNCTION)
		return Get_user_function(lexeme_array, curr_lex, func_list);

	else if(tmp_data->expression_type == VAR) { // Get_Id

		Node_data* tmp_data2 = NULL;
		memcpy(&tmp_data2, &lexeme_array->lex_arr[*curr_lex+1].root->node_data, sizeof(Node_data*));
		if(tmp_data2->expression_type == PARENTHESES_OP)
			tmp_data->expression_type = FUNCTION;

		else {

			INSERT_LOC_VAR
		}

		(*curr_lex)++;

		return lexeme_array->lex_arr[*curr_lex - 1].root;
	}

	else
		return NULL;
}

Node* Get_number(Lexeme_array* lexeme_array, int64_t* curr_lex) {

	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));
	(*curr_lex)++;

	return _NUM(tmp_data->value);
}

Node* Get_function(Lexeme_array* lexeme_array, int64_t* curr_lex, Func_data_list* func_list) {

	Node_data* tmp_data = NULL;
	memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	if(tmp_data->value == LOG) {

		int64_t func_index = (*curr_lex);
		(*curr_lex)++;
		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

		if(tmp_data->value != PARENTHESES_OP || tmp_data->expression_type != SPECIAL_SYMBOL)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		(*curr_lex)++;

		Node* left_node = Get_addition_priority(lexeme_array, curr_lex, func_list);

		if(!left_node)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

		if(tmp_data->value != COMMA || tmp_data->expression_type != SPECIAL_SYMBOL)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		(*curr_lex)++;
		Node* right_node = Get_addition_priority(lexeme_array, curr_lex, func_list);

		if(!right_node)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

		if(tmp_data->value != PARENTHESES_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		(*curr_lex)++;

		lexeme_array->lex_arr[func_index].root->left_node = left_node;
		lexeme_array->lex_arr[func_index].root->right_node = right_node;

		return lexeme_array->lex_arr[func_index].root;
	}

	else {

		int64_t func_index = (*curr_lex);
		(*curr_lex)++;

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

		if(tmp_data->value != PARENTHESES_OP || tmp_data->expression_type != SPECIAL_SYMBOL)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		(*curr_lex)++;
		Node* left_node = Get_addition_priority(lexeme_array, curr_lex, func_list);

		if(!left_node)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		memcpy(&tmp_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

		if(tmp_data->value != PARENTHESES_CL || tmp_data->expression_type != SPECIAL_SYMBOL)
			SYNTAX_ERROR(lexeme_array, curr_lex);

		(*curr_lex)++;
		lexeme_array->lex_arr[func_index].root->left_node = left_node;

		return lexeme_array->lex_arr[func_index].root;
	}
}

void Syntax_error(Lexeme_array* lexeme_array, int64_t* curr_lex, const char* func, int line) {

	Node_data* tmp_node_data = NULL;
	memcpy(&tmp_node_data, &lexeme_array->lex_arr[*curr_lex].root->node_data, sizeof(Node_data*));

	DEBUG_PRINTF("Syntax Error in function '%s', line %d:\n", func, line);
	DEBUG_PRINTF("line: %d,", lexeme_array->lex_arr[*curr_lex].line);
	DEBUG_PRINTF("col: %d\n", lexeme_array->lex_arr[*curr_lex].col);
	DEBUG_PRINTF("lexeme: ");

	switch(tmp_node_data->expression_type) {
        case NUM: {

			DEBUG_PRINTF("expression_type = %#X (NUM), \n", NUM);
            double tmp_double = 0;
            memcpy(&tmp_double, &tmp_node_data->value, sizeof(double));

            if(fabs(tmp_double - M_PI) < Epsilon) {

                DEBUG_PRINTF("value = pi");
			}

            else {

				if(fabs(tmp_double - M_E) < Epsilon) {

                	DEBUG_PRINTF("value = e");
				}

				else
					DEBUG_PRINTF("value = %.4lg", tmp_double);
			}

            break;
        }

        case VAR: {

            DEBUG_PRINTF("expression_type = %#X (VAR), \n", VAR);
            Variable_data* tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(Variable_data*));
            DEBUG_PRINTF("value = %.*s", (size_t) tmp_int->var_len, tmp_int->var_name);
            break;
        }

        case OP: {

            DEBUG_PRINTF("expression_type = %#X (OP), \n", OP);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define FUNC(func_name, func_full_name) case func_name:\
                                                               DEBUG_PRINTF("value = %s ", func_full_name);\
															   DEBUG_PRINTF("(%#X)", func_name);\
                                                               break;

				#define FUNC_BASIC(func_name, func_full_name) case func_name:\
                                                               DEBUG_PRINTF("value = %s ", func_full_name);\
															   DEBUG_PRINTF("(%#X)", func_name);\
                                                               break;

                #include "Funcs_code_gen.h"
                #undef FUNC
				#undef FUNC_BASIC

                default:

                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }
            DEBUG_PRINTF("\n");
            if(!tmp_int)
                DEBUG_PRINTF("ERROR: wrong expression type (0 operation)\n");

            break;

        }

        case FUNCTION: {

            DEBUG_PRINTF("expression_type = %#X (FUNCTION), \n", FUNCTION);
            Variable_data* tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(Variable_data*));
            DEBUG_PRINTF("value = %.*s", (size_t) tmp_int->var_len, tmp_int->var_name);
            break;

        }

        case CONDITION: {

            DEBUG_PRINTF("expression_type = %#X (CONDITION), \n", CONDITION);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define CONDITION(value, symbol, spu_code) case value:\
                                                    DEBUG_PRINTF("value = %s ", symbol);\
													DEBUG_PRINTF("(%#X)", value);\
                                                    break;

                #include "Conditions_code_gen.h"
                #undef FUNC

                default:
                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }

			break;
        }

        case SPECIAL_SYMBOL: {

            DEBUG_PRINTF("expression_type = %#X (SPECIAL_SYMBOL), \n", SPECIAL_SYMBOL);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define SPECIAL_SYMBOL(value, symbol) case value:\
                                                          DEBUG_PRINTF("value = %c ", symbol);\
														  DEBUG_PRINTF("(%#X)", value);\
                                                          break;

                #include "Special_symbols_code_gen.h"
                #undef FUNC

                default:
                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }

			break;
        }

        case KEY_WORD: {

            DEBUG_PRINTF("expression_type = %#X (KEY_WORD), \n", KEY_WORD);
            int64_t tmp_int = 0;
            memcpy(&tmp_int, &tmp_node_data->value, sizeof(int64_t));
            switch(tmp_int) {

                #define KEY_WORD(value, symbol) case value:\
                                                    DEBUG_PRINTF("value = %s ", symbol);\
													DEBUG_PRINTF("(%#X)", value);\
                                                    break;

                #include "Key_words_code_gen.h"
                #undef FUNC

                default:
                    DEBUG_PRINTF("ERROR: wrong expression value = %#X \n", tmp_int);
            }

			break;
        }

        case IF_BRANCHES: {

            DEBUG_PRINTF("expression_type = %#X (IF_BRANCHES), \n", IF_BRANCHES);
            DEBUG_PRINTF("value = CONNECTOR");
			break;
        }

        default:
            DEBUG_PRINTF("ERROR: wrong expression type\n %#X", tmp_node_data->expression_type);
    }

	abort();
}

bool Lang_get_next_symbol(const char* node_data, uint64_t data_size,
						  int64_t* curr_pos, int64_t* line, int64_t* col) {

	while(*curr_pos < data_size && node_data[*curr_pos] != '\n' &&
		  node_data[*curr_pos] != '\t' && isspace(node_data[*curr_pos])) {

		(*col)++;
		(*curr_pos)++;
	}

	if((*curr_pos) >= data_size)
		return false;

	return true;
}
