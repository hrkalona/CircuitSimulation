#include "circuit_simulation.h" 

struct twoTerminalsElement *head_twoTerminalsElement_list = NULL, *last_twoTerminalsElement_list = NULL;
struct threeTerminalsElement *head_threeTerminalsElement_list = NULL, *last_threeTerminalsElement_list = NULL;
struct fourTerminalsElement *head_fourTerminalsElement_list = NULL, *last_fourTerminalsElement_list = NULL;
struct hash_node * head_hash_node_list[HASH_TABLE_SIZE];

int ground_found = 0;


void parseInput(FILE *p_file) {
  char line[SIZE_OF_LINE];
  char *temp;
  struct twoTerminalsElement *current1 = NULL;
  struct threeTerminalsElement *current2 = NULL;
  struct fourTerminalsElement *current3 = NULL;
  int counter = 1, i, j, temp_group2 = 0, temp3;
  
     
     printf("\n\nInf-Uth Spice starting!\n\n");
     
     printf("\n\nParsing...\n\n");
     
  
     /*read the next line of the file */
     while(fgets(line, SIZE_OF_LINE, p_file) != NULL) {
       /*ignore the space and tab characters at the beginning of the line */
        for(i = 0; (line[i] == ' ') || (line[i] == '\t'); i++);
	/*select the type of the element */
	switch (line[i]) {
	    /* voltage source */
	    case 'v':
	    case 'V':
	    
		current1 = (struct twoTerminalsElement*) malloc(sizeof(struct twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		group2_elements++;
	      
		current1 -> type = VOLTAGE_SOURCE;
	    	
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++number_of_elements[VOLTAGE_SOURCE];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = hash(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = hash(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = atof(temp);

		temp = strtok(NULL, "\t \n(,");
		
		if(temp == NULL || temp[0] == 13) {
		    current1 -> transient  = NULL;
		}
		else {
		  
		    current1 -> transient = (struct transientComponent*) malloc(sizeof(struct transientComponent));
		    
		    if(current1 -> transient == NULL) {
			printf("Could not allocate new node.\n");
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    if(!strcasecmp(temp, "EXP")) {
			current1 -> transient -> impulse = EXP;
			
			current1 -> transient -> exp = (struct ExpImpulse*) malloc(sizeof(struct ExpImpulse));
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = NULL;
			
			if(current1 -> transient -> exp == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> i1 = atof(temp);
			
			if(fabs(current1 -> value - current1 -> transient -> exp -> i1) > ZERO) {
			    printf("The first argument of exp and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> i2 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> td1 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc1 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> exp -> td2 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc2 = atof(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
		    
			if(temp != NULL && temp[0] != 13) {
			    printf("Too many inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}

		    }
		    else if(!strcasecmp(temp, "SIN")) {
			current1 -> transient -> impulse = SIN;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = (struct SinImpulse*) malloc(sizeof(struct SinImpulse));
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = NULL;
			
			if(current1 -> transient -> sin == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> i1 = atof(temp);
			
			if(fabs(current1 -> value - current1 -> transient -> sin -> i1) > ZERO) {
			    printf("The first argument of sin and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> ia = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> fr = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> td = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> sin -> df = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> ph = atof(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
		    
			if(temp != NULL && temp[0] != 13) {
			    printf("Too many inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
		    
		    }
		    else if(!strcasecmp(temp, "PULSE")) {
			current1 -> transient -> impulse = PULSE;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = (struct PulseImpulse*) malloc(sizeof(struct PulseImpulse));
			current1 -> transient -> pwl = NULL;
			
			if(current1 -> transient -> pulse == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> i1 = atof(temp);
			
			if(fabs(current1 -> value - current1 -> transient -> pulse -> i1) > ZERO) {
			    printf("The first argument of pulse and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> i2 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> td = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> tr = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> pulse -> tf = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> pw = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> per = atof(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
		    
			if(temp != NULL && temp[0] != 13) {
			    printf("Too many inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
		    
		    }
		    else if(!strcasecmp(temp, "PWL")) {
			current1 -> transient -> impulse = PWL;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = (struct PWLImpulse*) malloc(sizeof(struct PWLImpulse));
			
			if(current1 -> transient -> pwl == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			j = 0;
			current1 -> transient -> pwl -> length = 0;
			
			current1 -> transient -> pwl -> t = (double *) malloc((j + 1) * sizeof(double));
			current1 -> transient -> pwl -> i = (double *) malloc((j + 1) * sizeof(double));
			
			if(current1 -> transient -> pwl -> t == NULL || current1 -> transient -> pwl -> i == NULL) {
			    printf("Could not allocate matrices.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			do {
			
			    current1 -> transient -> pwl -> t = realloc(current1 -> transient -> pwl -> t, (j + 1) * sizeof(double));
			    current1 -> transient -> pwl -> i = realloc(current1 -> transient -> pwl -> i, (j + 1) * sizeof(double));
			    
			    if(current1 -> transient -> pwl -> t == NULL || current1 -> transient -> pwl -> i == NULL) {
				printf("Could not allocate matrices.\n");
				printf("Terminating.\n");
				exit(-1);
			    }

			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pwl -> t[j] = atof(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pwl -> i[j] = atof(temp);
			    			    
			    j++;
			    
			    current1 -> transient -> pwl -> length++;
			    
			    temp = strtok(NULL, "\t (),");
			    
			} while(temp != NULL && temp[0] != 13 && temp[0] != 10);
			
			current1 -> transient -> pwl -> t[0] = 0;
			
			if(current1 -> transient -> pwl -> length < 2) {
			    printf("The inputs for the piece wise linear need to be at least 2, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			if(fabs(current1 -> value - current1 -> transient -> pwl -> i[0]) > ZERO) {
			    printf("The first argument of pwl and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}

		    }
		    else {
			printf("Cannot parse line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
 
		}
	
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
		    non_zeroes_G += 2;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_G += 2;
		}
		
		break;
		
	    /* current source */	
	    case 'i':
	    case 'I':
	    
		current1 = (struct twoTerminalsElement*) malloc(sizeof(struct twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		group1_elements++;
	      
		current1 -> type = CURRENT_SOURCE;
	    	     
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++number_of_elements[CURRENT_SOURCE];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = hash(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = hash(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = atof(temp);
		
		
		temp = strtok(NULL, "\t \n(,");
		
		if(temp == NULL || temp[0] == 13) {
		    current1 -> transient  = NULL;
		}
		else {
		  
		    current1 -> transient = (struct transientComponent*) malloc(sizeof(struct transientComponent));
		    
		    if(current1 -> transient == NULL) {
			printf("Could not allocate new node.\n");
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    if(!strcasecmp(temp, "EXP")) {
			current1 -> transient -> impulse = EXP;
			
			current1 -> transient -> exp = (struct ExpImpulse*) malloc(sizeof(struct ExpImpulse));
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = NULL;
			
			if(current1 -> transient -> exp == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> i1 = atof(temp);
			
			if(fabs(current1 -> value - current1 -> transient -> exp -> i1) > ZERO) {
			    printf("The first argument of exp and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> i2 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> td1 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc1 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> exp -> td2 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc2 = atof(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
		    
			if(temp != NULL && temp[0] != 13) {
			    printf("Too many inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}

		    }
		    else if(!strcasecmp(temp, "SIN")) {
			current1 -> transient -> impulse = SIN;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = (struct SinImpulse*) malloc(sizeof(struct SinImpulse));
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = NULL;
			
			if(current1 -> transient -> sin == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> i1 = atof(temp);
			
			if(fabs(current1 -> value - current1 -> transient -> sin -> i1) > ZERO) {
			    printf("The first argument of sin and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> ia = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> fr = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> td = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> sin -> df = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> ph = atof(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
		    
			if(temp != NULL && temp[0] != 13) {
			    printf("Too many inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
		    
		    }
		    else if(!strcasecmp(temp, "PULSE")) {
			current1 -> transient -> impulse = PULSE;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = (struct PulseImpulse*) malloc(sizeof(struct PulseImpulse));
			current1 -> transient -> pwl = NULL;
			
			if(current1 -> transient -> pulse == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> i1 = atof(temp);
			
			if(fabs(current1 -> value - current1 -> transient -> pulse -> i1) > ZERO) {
			    printf("The first argument of pulse and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> i2 = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> td = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> tr = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> pulse -> tf = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> pw = atof(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> per = atof(temp);
			
			current1 -> transient -> vals = NULL;
					
			temp = strtok(NULL, "\t \n),");
	
		    
			if(temp != NULL && temp[0] != 13) {
			    printf("Too many inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
		    
		    }
		    else if(!strcasecmp(temp, "PWL")) {
			current1 -> transient -> impulse = PWL;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = (struct PWLImpulse*) malloc(sizeof(struct PWLImpulse));
			
			if(current1 -> transient -> pwl == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			j = 0;
			current1 -> transient -> pwl -> length = 0;
			
			current1 -> transient -> pwl -> t = (double *) malloc((j + 1) * sizeof(double));
			current1 -> transient -> pwl -> i = (double *) malloc((j + 1) * sizeof(double));
			
			if(current1 -> transient -> pwl -> t == NULL || current1 -> transient -> pwl -> i == NULL) {
			    printf("Could not allocate matrices.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			temp = strtok(NULL, "\t (),");
			
			do {
			
			    current1 -> transient -> pwl -> t = realloc(current1 -> transient -> pwl -> t, (j + 1) * sizeof(double));
			    current1 -> transient -> pwl -> i = realloc(current1 -> transient -> pwl -> i, (j + 1) * sizeof(double));
			    
			    if(current1 -> transient -> pwl -> t == NULL || current1 -> transient -> pwl -> i == NULL) {
				printf("Could not allocate matrices.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pwl -> t[j] = atof(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pwl -> i[j] = atof(temp);
			    			    
			    j++;
			    
			    current1 -> transient -> pwl -> length++;
			    
			    temp = strtok(NULL, "\t (),");
			    
			} while(temp != NULL && temp[0] != 13 && temp[0] != 10);
			
			current1 -> transient -> pwl -> t[0] = 0;
			
			if(current1 -> transient -> pwl -> length < 2) {
			    printf("The inputs for the piece wise linear need to be at least 2, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			if(fabs(current1 -> value - current1 -> transient -> pwl -> i[0]) > ZERO) {
			    printf("The first argument of pwl and value must be the same, at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}

		    }
		    else {
			printf("Cannot parse line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
 
		}
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		break;
		
	    /* resistance */	
	    case 'r':
	    case 'R':
	    
		current1 = (struct twoTerminalsElement*) malloc(sizeof(struct twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		group1_elements++;
	      
		current1 -> type = RESISTANCE;
	    
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++number_of_elements[RESISTANCE];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = hash(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = hash(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = atof(temp);
		
		temp = strtok(NULL, "\t \n");
		
		if(temp != NULL && temp[0] != 13) {
		    printf("Too many inputs at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		
		current1 -> transient = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
	            non_zeroes_G++;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_G++;
		}
		
		if(current1 -> positive_terminal && current1 -> negative_terminal) {
		    non_zeroes_G += 2;
		}
		
		break;
		
	    /* capacitor */	
	    case 'c':
	    case 'C':
	    
		current1 = (struct twoTerminalsElement*) malloc(sizeof(struct twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		group1_elements++;
	      
		current1 -> type = CAPACITOR;
	    
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++number_of_elements[CAPACITOR];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = hash(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = hash(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = atof(temp);
		
		temp = strtok(NULL, "\t \n");
		
		if(temp != NULL && temp[0] != 13) {
		    printf("Too many inputs at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		current1 -> transient = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
	            non_zeroes_C++;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_C++;
		}
		
		if(current1 -> positive_terminal && current1 -> negative_terminal) {
		    non_zeroes_C += 2;
		}
		
		break;
		
	    /* inductor*/	
	    case 'l':
	    case 'L':
	    
		current1 = (struct twoTerminalsElement*) malloc(sizeof(struct twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		group2_elements++;
	      
		current1 -> type = INDUCTOR;
	    
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++number_of_elements[INDUCTOR];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = hash(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = hash(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = atof(temp);
		
		temp = strtok(NULL, "\t \n");
		
		if(temp != NULL && temp[0] != 13) {
		    printf("Too many inputs at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		current1 -> transient = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
		    non_zeroes_G += 2;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_G += 2;
		}
		
		non_zeroes_C++;
		
		break;
		
	    /* diode */	
	    case 'd':
	    case 'D':
	    
		current1 = (struct twoTerminalsElement*) malloc(sizeof(struct twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> type = DIODE;
	   	      
		strcpy(current1 -> string_name, strtok(line + i + 1, "\t "));
		
		current1 -> name = ++number_of_elements[DIODE];
	   	      
		current1 -> positive_terminal = hash(strtok(NULL, "\t "));
	      
		current1 -> negative_terminal = hash(strtok(NULL, "\t "));
		      
		strtok(NULL, "\t ");  //MODEL NAME(ignored)	    
	      
		temp = strtok(NULL, "\t \n");
		//the field area is optional
		if(temp == NULL) {
		    current1 -> value = 1;
		}
		else {
		    current1 -> value = atof(temp); 
		}
		
		current1 -> transient = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
	      
		break;
		
	    /* bjt transistor */	
	    case 'q':
	    case 'Q':
	      
		current2 = (struct threeTerminalsElement*) malloc(sizeof(struct threeTerminalsElement));
		
		if(current2 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		  
		current2 -> type = BJT;
		 	      
		strcpy(current2 -> string_name, strtok(line + i + 1, "\t "));
		
		current2 -> name = ++number_of_elements[BJT];
			      
		current2 -> c_terminal = hash(strtok(NULL, "\t "));
			      
		current2 -> b_terminal = hash(strtok(NULL, "\t "));
			      
		current2 -> e_terminal = hash(strtok(NULL, "\t "));
		
		strtok(NULL, "\t ");  //MODEL NAME(ignored)
		
		temp = strtok(NULL, "\t \n");
		//the field area is optional
		if(temp == NULL) {
		    current2 -> value = 1;
		}
		else {
		    current2 -> value = atof(temp); 
		}
	      
		current2 -> next = NULL;
		
		insertThreeTerminalsElement(current2);
	      
		break;
		
	    /* mos transistor */	
	    case 'm':
	    case 'M':
	      
		current3 = (struct fourTerminalsElement*) malloc(sizeof(struct fourTerminalsElement));
		
		if(current3 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		  
		current3 -> type = MOS;
		  	      
		strcpy(current3 -> string_name, strtok(line + i + 1, "\t "));
		
		current3 -> name = ++number_of_elements[MOS];
	      
		current3 -> d_terminal = hash(strtok(NULL, "\t "));
	      
		current3 -> g_terminal = hash(strtok(NULL, "\t "));
	      
		current3 -> s_terminal = hash(strtok(NULL, "\t "));
	      
		current3 -> b_terminal = hash(strtok(NULL, "\t "));
		
		strtok(NULL, "\t ");  //MODEL NAME(ignored)
		
		current3 -> length = atof(strtok(NULL, "\t lL="));
		
		current3 -> width = atof(strtok(NULL, "\t wW=\n"));
	      
		current3 -> next = NULL;
		
		insertFourTerminalsElement(current3);
	      
		break;
		
	    /* comment line */	
	    case '*':
	    case 13:
	    case '\n':
	      
	        break;
		
	    case '.':
	      
	        temp = strtok(line + i + 1, "\t \n");
		
		if(temp != NULL) {
		    for(i = 0; i < strlen(temp); i++) {
			if(temp[i] == 13) {
			    temp[i] = '\0'; 
			}
		    }
		}
		
		if(!strcasecmp(temp, "OPTIONS")) {
		    do {
			temp = strtok(NULL, "\t \n=");
			
			if(temp != NULL) {
			    for(i = 0; i < strlen(temp); i++) {
			        if(temp[i] == 13) {
				    temp[i] = '\0'; 
				}
			    }
			}
				
			if(temp == NULL || temp[0] == '\0') {
			    break;
			}
			
			if(!strcasecmp(temp, "SPD")) {
			    matrix_type = SPD;
			    printf("Options: Symmetric & Positive Matrices Activated.\n");
			}
			else if(!strcasecmp(temp, "ITER")) {
			    method = ITERATIVE;
			    printf("Options: Iterative Methods Activated.\n");
			}
			else if(!strcasecmp(temp, "ITOL")) {
			    temp = strtok(NULL, "\t \n=");
			    if(temp == NULL) {
			        printf("Cannot parse line %d.\n", counter);
			        printf("Terminating.\n");
			        exit(-1); 
			    }
			    itol = atof(temp);
			    printf("Options: Iterative Threshold set to %.8e .\n", itol);
			}
			else if(!strcasecmp(temp, "SPARSE")) {
			    matrix_sparsity = SPARSE;
			    printf("Options: Sparse Matrices Activated.\n");
			}
			else if(!strcasecmp(temp, "METHOD")) {
			    temp = strtok(NULL, "\t \n=");
			    if(temp == NULL) {
			        printf("Cannot parse line %d.\n", counter);
			        printf("Terminating.\n");
			        exit(-1); 
			    }

			    for(i = 0; i < strlen(temp); i++) {
			        if(temp[i] == 13) {
				    temp[i] = '\0'; 
				}
			    }
			
			    if(!strcasecmp(temp, "TR")) {
				diff_method = TRAPEZOIDAL;
				printf("Options: Trapezoidal Method Activated.\n");
			      
			    }
			    else if(!strcasecmp(temp, "BE")) {
				diff_method = EULER;
				printf("Options: Backward Euler Method Activated.\n");
			    }
			    else {
				printf("Cannot parse line %d.\n", counter);
			        printf("Terminating.\n");
			        exit(-1); 
			    }
			}
			else if(!strcasecmp(temp, "GNUPLOTLP")) {
			    gnuplot = LINES_POINTS;
			    printf("Options: GNU plot Activated (Lines and Points).\n");
			}
			else if(!strcasecmp(temp, "GNUPLOTL")) {
			    gnuplot = LINES;
			    printf("Options: GNU plot Activated (Lines).\n");
			}
			else {
			    printf("Cannot parse line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
		    } while(1);
		}
		else if(!strcasecmp(temp, "DC")) {
	            temp = strtok(NULL, "\t ");
		    
		    if(temp == NULL || temp[0] == 13) {
		        printf("DC sweep element type and name not found at line %d.\n", counter);
		        printf("Terminating.\n");
			exit(-1);
		    }
		    
		    switch (temp[0]) {
		        case 'V':
		        case 'v':
			    temp++;
			    for(current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
			        if(current1 -> type == VOLTAGE_SOURCE || current1 -> type == INDUCTOR) {
				    temp_group2++;
				}
			        if(current1 -> type == VOLTAGE_SOURCE && !strcasecmp(temp, current1 -> string_name)) {
                                    break;
				}
			    }
			    
			    
			    dc_sweep_index1 = -1;
                            dc_sweep_index2 = -1;
			    
			    if(current1 == NULL) {
			        printf("\n\nThe element, V%s does not exist, at line %d.\n", temp, counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    dc_sweep = SWEEP_VOLTAGE_SOURCE;
			    dc_sweep_index1 = temp_group2 - 1;
			    
			    temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep start value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    dc_sweep_start = atof(temp);
			    
			    temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep end value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                            dc_sweep_end = atof(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep step value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                            dc_sweep_step = atof(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    if(dc_sweep_start == dc_sweep_end) {
			        printf("\n\nThe start and stop value must no be equal, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    			    
			    if(dc_sweep_step == 0) {
			        printf("\n\nThe sweep step must not be 0, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if(dc_sweep_start < dc_sweep_end && dc_sweep_step < 0) {
			        printf("\n\nThe sweep step must be greater than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if(dc_sweep_start > dc_sweep_end && dc_sweep_step > 0) {
			        printf("\n\nThe sweep step must be less than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    sweep_name[0] = '\0';
			    strcat(sweep_name, "V ");
			    strcat(sweep_name, current1 -> string_name);
	
			    break;
			  
			case 'I':
			case 'i':
			    temp++;
			    for(current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {	  
			        if(current1 -> type == CURRENT_SOURCE && !strcasecmp(temp, current1 -> string_name)) {
                                    break;
				}
			    }
			    
			    dc_sweep_index1 = -1;
                            dc_sweep_index2 = -1;
			    
			    if(current1 == NULL) {
			        printf("\n\nThe element, I%s does not exist, at line %d.\n", temp, counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    dc_sweep = SWEEP_CURRENT_SOURCE;
			    
			    if(current1 -> positive_terminal) {
		                 dc_sweep_index1 = current1 -> positive_terminal - 1;
		            }
		
		            if(current1 -> negative_terminal) {
		                 dc_sweep_index2 = current1 -> negative_terminal - 1;
		            }
		            
		            temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep start value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    dc_sweep_start = atof(temp);
			    
			    temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep end value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                            dc_sweep_end = atof(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep step value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                            dc_sweep_step = atof(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }			    
			    
			    if(dc_sweep_start == dc_sweep_end) {
			        printf("\n\nThe start and stop value must not be equal, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if(dc_sweep_step == 0) {
			        printf("\n\nThe sweep step must not be 0, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }			    
			    
			    if(dc_sweep_start < dc_sweep_end && dc_sweep_step < 0) {
			        printf("\n\nThe sweep step must be greater than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if(dc_sweep_start > dc_sweep_end && dc_sweep_step > 0) {
			        printf("\n\nThe sweep step must be less than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    sweep_name[0] = '\0';
			    strcat(sweep_name, "I ");
			    strcat(sweep_name, current1 -> string_name);
               
			    break;			    
			  
			default:
			     printf("Cannot parse line %d.\n", counter);
		             printf("Terminating.\n");
		             exit(-1);
		      
		    }
		}
		else if(!strcasecmp(temp, "PLOT") || !strcasecmp(temp, "PRINT")) {
		  
		    temp = strtok(NULL, "\t ");
		    
		    if(!strcasecmp(temp, "DC")) {
			do {
			    temp = strtok(NULL, "\t vV()\n");
			  
			    if(temp != NULL) {
				for(i = 0; i < strlen(temp); i++) {
				    if(temp[i] == 13) {
					temp[i] = '\0'; 
				    }
				}
			    }
			  
			    if (temp == NULL || temp[0] == '\0') {
				break;
			    }
			  
			    temp3 = search_for_node(temp);
			    
			    if(temp3 == -1) {
				printf("\n\nThe node, %s does not exist, at line %d.\n", temp, counter);
				printf("Terminating.\n");
				exit(-1); 
			    }
			    
			    node_dc_plot_vector[temp3] = 1;
			} while(1);
		    }
		    else if(!strcasecmp(temp, "TRAN")) {
			do {
			    temp = strtok(NULL, "\t vV()\n");
			  
			    if(temp != NULL) {
				for(i = 0; i < strlen(temp); i++) {
				    if(temp[i] == 13) {
					temp[i] = '\0'; 
				    }
				}
			    }
			  
			    if (temp == NULL || temp[0] == '\0') {
				break;
			    }
			  
			    temp3 = search_for_node(temp);
			    
			    if(temp3 == -1) {
				printf("\n\nThe node, %s does not exist, at line %d.\n", temp, counter);
				printf("Terminating.\n");
				exit(-1); 
			    }
			    
			    node_transient_plot_vector[temp3] = 1;
			} while(1);
		    }
		    else {
			printf("Expecting plot mode (DC or TRAN), at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }  
		}
		else if(!strcasecmp(temp, "TRAN")) {
		    transient_analysis = 1;
		    
		    temp = strtok(NULL, "\t ");
		    if(temp == NULL || temp[0] == 13) {
			printf("Transient time step value not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
		    time_step = atof(temp);
		    
		    temp = strtok(NULL, "\t \n");
		    if(temp == NULL || temp[0] == 13) {
			printf("Transient finish time value not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
                    fin_time = atof(temp);
		    
		    temp = strtok(NULL, "\t \n");
			    
		    if(temp != NULL && temp[0] != 13) {
			printf("Too many inputs at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }		
		    
		    if(time_step <= 0) {
			 printf("\n\nThe time step must be greater than 0, at line %d.\n", counter);
		         printf("Terminating.\n");
		         exit(-1); 
		    }
		    
		    if(fin_time <= 0) {
			 printf("\n\nThe finish time must be greater than 0, at line %d.\n", counter);
		         printf("Terminating.\n");
		         exit(-1); 
		    }
		    
		    if(time_step >= fin_time) {
			printf("\n\nThe time step must lower than finish time, at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1); 
		    }
	
		}
		else if(!strcasecmp(temp, "OP")) {
		    operating_point = 1; // for printing currents of the elements at dc analysis
		}
		else {
		    printf("Cannot parse line %d.\n", counter);
		    printf("Terminating.\n");
		    exit(-1);
		}
		
	        break;
		
		
	    default:
	        printf("Cannot parse line %d.\n", counter);
		printf("Terminating.\n");
		exit(-1);
	  
	}
	
	counter++;
	
    }
    /* check whether the ground node found .if not terminate the simulation*/
    if(!ground_found) {
        printf("Ground node not found.\n");
	printf("Terminating.\n");
	exit(-1);   
    }
    
    printf("\n\nParsing results. The circuit contains:\n\n");

    printf("          Nodes: %7d\n", number_of_nodes + 1);
    
    if(number_of_elements[VOLTAGE_SOURCE]) {
	printf("Voltage Sources: %7d\n", number_of_elements[VOLTAGE_SOURCE]);
    }
    
    if(number_of_elements[CURRENT_SOURCE]) {
	printf("Current Sources: %7d\n", number_of_elements[CURRENT_SOURCE]);
    }
    
    if(number_of_elements[RESISTANCE]) {
	printf("    Resistances: %7d\n", number_of_elements[RESISTANCE]);
    }
    
    if(number_of_elements[CAPACITOR]) {
	printf("     Capacitors: %7d\n", number_of_elements[CAPACITOR]);
    }
    
    if(number_of_elements[INDUCTOR]) {
	printf("      Inductors: %7d\n", number_of_elements[INDUCTOR]);
    }
    
    if(number_of_elements[DIODE]) {
	printf("         Diodes: %7d\n", number_of_elements[DIODE]);
    }
    
    if(number_of_elements[BJT]) {
	printf("BJT Transistors: %7d\n", number_of_elements[BJT]);
    }
    
    if(number_of_elements[MOS]) {
	printf("MOS Transistors: %7d\n", number_of_elements[MOS]);
    }
    
    printf("\n");
    
    #if DEBUG
    printf("\n\nTWO TERMINALS ELEMENTS:\n\n");
    for(current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
	if(current1 -> transient == NULL) {
	    printf("TYPE: %d  ORIGINAL NAME: %9s  ASSIGNED NAME: %d  POSITIVE: %d  NEGATIVE: %d  VALUE: %.14lf\n", current1 -> type, current1 -> string_name, current1 -> name, current1 -> positive_terminal, current1 -> negative_terminal, current1 -> value);
	}
	else {
	    printf("TYPE: %d  ORIGINAL NAME: %9s  ASSIGNED NAME: %d  POSITIVE: %d  NEGATIVE: %d  VALUE: %.14lf  IMPULSE: %d\n", current1 -> type, current1 -> string_name, current1 -> name, current1 -> positive_terminal, current1 -> negative_terminal, current1 -> value, current1 -> transient -> impulse);
	}
    }
    printf("\n\n");
    
    
    printf("THREE TERMINALS ELEMENTS:\n\n");
    for(current2 = head_threeTerminalsElement_list; head_threeTerminalsElement_list != NULL && current2 != NULL; current2 = current2 -> next) {
        printf("TYPE: %d  ORIGINAL NAME: %9s  ASSIGNED NAME: %d C: %d  B: %d  E: %d  VALUE: %.14lf\n", current2 -> type, current2 -> string_name, current2 -> name, current2 -> c_terminal, current2 -> b_terminal, current2 -> e_terminal, current2 -> value);
    }
    printf("\n\n");
    
    printf("FOUR TERMINALS ELEMENTS:\n\n");
    for(current3 = head_fourTerminalsElement_list; head_fourTerminalsElement_list != NULL && current3 != NULL; current3 = current3 -> next) {
        printf("TYPE: %d  ORIGINAL NAME: %9s  ASSIGNED NAME: %d D: %d  G: %d  S: %d  B: %d  LENGTH: %.14lf  WIDTH: %.14lf\n", current3 -> type, current3 -> string_name, current3 -> name, current3 -> d_terminal, current3 -> g_terminal, current3 -> s_terminal, current3 -> b_terminal, current3 -> length, current3 -> width);
    }
    printf("\n\n");
    #endif   
    
}


void insertTwoTerminalsElement(struct twoTerminalsElement *current) {
 
    if(head_twoTerminalsElement_list == NULL) {
        head_twoTerminalsElement_list = current;
	last_twoTerminalsElement_list = current;
    }
    else {
        last_twoTerminalsElement_list -> next = current;
	last_twoTerminalsElement_list = current;	
    }
    
}


void insertThreeTerminalsElement(struct threeTerminalsElement *current) { 
 
    if(head_threeTerminalsElement_list == NULL) {
        head_threeTerminalsElement_list = current;
	last_threeTerminalsElement_list = current;
    }
    else {
        last_threeTerminalsElement_list -> next = current;
	last_threeTerminalsElement_list = current;	
    }
    
}


void insertFourTerminalsElement(struct fourTerminalsElement *current) {
 
    if(head_fourTerminalsElement_list == NULL) {
        head_fourTerminalsElement_list = current;
	last_fourTerminalsElement_list = current;
    }
    else {
        last_fourTerminalsElement_list -> next = current;
	last_fourTerminalsElement_list = current;	
    }
    
}


int hash(char *string) {
  struct hash_node *temp = NULL, *current = NULL, *last = NULL;
  int i, temp2;
  unsigned int hash_code = 5381;
  
  
    /*check whether the node is the ground */
    if(!strcmp(string, "0")) {
        ground_found = 1;
        return 0;
    }
      
    for(i = 0; string[i] != '\0'; i++){ //Convert to lower Case and calculate the hash code
        string[i] = tolower(string[i]);
	hash_code += (hash_code << 5) + hash_code + string[i]; //DJB
    }
       
    temp2 = hash_code % HASH_TABLE_SIZE;  // The node will be in one of the lists (0 - 1023)
    
    if(head_hash_node_list[temp2] == NULL) { // The corresponding list is empty
	current = (struct hash_node*) malloc(sizeof(struct hash_node)); // allocate new node for it
		  
	if(current == NULL) {
	    printf("Could not allocate new node.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	strcpy(current -> name, string); // copy its name
	current -> mapping = ++number_of_nodes; // get the next number for mapping
	current -> next = NULL;
	
	#if DEBUG
	printf("Node Mapping, %s -> %d.\n", current -> name, current -> mapping);
        #endif
	
	node_real_names = (char *) realloc(node_real_names, (number_of_nodes + 1) * 256 * sizeof(char));  // the inverse mapping, number to name, O(1) access
	
	if(node_real_names == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	strcpy(node_real_names + number_of_nodes * 256, current -> name);
	
	node_dc_plot_vector = (int *) realloc(node_dc_plot_vector, (number_of_nodes + 1) * sizeof(int));  // dc sweep plot vector
	
	if(node_dc_plot_vector == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	node_dc_plot_vector[number_of_nodes] = 0;
	
	node_transient_plot_vector = (int *) realloc(node_transient_plot_vector, (number_of_nodes + 1) * sizeof(int)); // transient analysis plot vector
	
	if(node_transient_plot_vector == NULL) {
	    printf("Could not allocate matrices.\n");
	    printf("Terminating.\n");
	    exit(-1);
	}
	
	node_transient_plot_vector[number_of_nodes] = 0;
	
	head_hash_node_list[temp2] = current; // add the node to the list
	
	return current -> mapping; // return its mapping
    }
    else { // the list is not empty
	for(temp = head_hash_node_list[temp2]; temp != NULL;  temp = temp -> next) { // search the whole list
	    if(!strcmp(string, temp -> name)) {
	        break; // already in the list
	    }
	    last = temp;
	}
	
	if(temp != NULL) {
	    return temp -> mapping; // already in the list, return its mapping
	}
	else { // not in the list
	    current = (struct hash_node*) malloc(sizeof(struct hash_node)); // allocate new node for it
		  
	    if(current == NULL) {
	        printf("Could not allocate new node.\n");
	        printf("Terminating.\n");
	        exit(-1);
	    }
	    	
	    strcpy(current -> name, string); // copy its name
	    current -> mapping = ++number_of_nodes; // get the next number for mapping
	    current -> next = NULL; 
	    
	    #if DEBUG
	    printf("Mapping, %s -> %d.\n", current -> name, current -> mapping);
            #endif
	    
	    node_real_names = (char *) realloc(node_real_names, (number_of_nodes + 1) * 256 * sizeof(char));  // the inverse mapping, number to name, O(1) access
	
	    if(node_real_names == NULL) {
		printf("Could not allocate matrices.\n");
		printf("Terminating.\n");
		exit(-1);
	    }
	    
	    strcpy(node_real_names + number_of_nodes * 256, current -> name);
	    
	    node_dc_plot_vector = (int *) realloc(node_dc_plot_vector, (number_of_nodes + 1) * sizeof(int));  // dc sweep plot vector
	    
	    if(node_dc_plot_vector == NULL) {
		printf("Could not allocate matrices.\n");
		printf("Terminating.\n");
		exit(-1);
	    }
	    
	    node_dc_plot_vector[number_of_nodes] = 0;
	    
	    node_transient_plot_vector = (int *) realloc(node_transient_plot_vector, (number_of_nodes + 1) * sizeof(int)); // transient analysis plot vector
	    
	    if(node_transient_plot_vector == NULL) {
		printf("Could not allocate matrices.\n");
		printf("Terminating.\n");
		exit(-1);
	    }
	    
	    node_transient_plot_vector[number_of_nodes] = 0;
	    
	    last -> next = current; // add the node to the list
	
	    return current -> mapping; // return its mapping
	}
    }
      
}

int search_for_node(char *string) { // this function is used for the plot/print options, to check if the requested node for plotting, is in the circuit
  struct hash_node *temp = NULL;
  int i, temp2;
  unsigned int hash_code = 5381;
  
     if(!strcmp(string, "0") && ground_found == 0) { // requested ground for plotting, but ground was not found yet
        return -1;
     }
     else if(!strcmp(string, "0") && ground_found == 1) { // ground was found, return its mapping
        return 0; 
     }
     
     for(i = 0; string[i] != '\0'; i++){ //Convert to lower Case and calculate the hash code
        string[i] = tolower(string[i]);
	hash_code += (hash_code << 5) + hash_code + string[i]; //DJB
     }
       
     temp2 = hash_code % HASH_TABLE_SIZE; // the node will be in one of the lists (0 - 1023)
     
     if(head_hash_node_list[temp2] == NULL) { // the corresponding list is empty
         return -1;
     }
     else { // the list is not empty
	for(temp = head_hash_node_list[temp2]; temp != NULL;  temp = temp -> next) {
	    if(!strcmp(string, temp -> name)) {
	        break; // found it
	    }
	}
	
	if(temp != NULL) {
	    return temp -> mapping; // found it, return is mapping
	}
	else {
	    return -1; // not found
	}
    }
  
}

