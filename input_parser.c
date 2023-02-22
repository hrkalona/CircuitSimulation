#include "circuit_simulation.h" 

int counter = 1;

void parseInput(FILE *p_file) {
  char line[SIZE_OF_LINE];
  char *temp;
  twoTerminalsElement *current1 = NULL;
  threeTerminalsElement *current2 = NULL;
  fourTerminalsElement *current3 = NULL;
  unsigned int i, j, k, temp_group2 = 0;
  int temp3;
  
     printf("\n\nInf-Uth Spice starting!\n\n");
     
     printf("\n\nParsing circuit...\n\n");

     
     /*read the next line of the file */
     while(fgets(line, SIZE_OF_LINE, p_file) != NULL) {
       /*ignore the space and tab characters at the beginning of the line */
        for(i = 0; (line[i] == ' ') || (line[i] == '\t'); i++);
	/*select the type of the element */
	switch (line[i]) {
	    /* voltage source */
	    case 'v':
	    case 'V':
	    
		current1 = (twoTerminalsElement*) malloc(sizeof(twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		circuit_simulation.group2_elements++;
	      
		current1 -> type = VOLTAGE_SOURCE;
                current1 -> isG2 = 1; 
	    	
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++circuit_simulation.number_of_elements[VOLTAGE_SOURCE];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = insert_node(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = insert_node(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}

		current1 -> value = invalid_number_checker(temp);

		temp = strtok(NULL, "\t \n(,");
		
		if(temp == NULL || temp[0] == 13) {
		    current1 -> transient = NULL;
		    current1 -> ac = NULL;
		}
		else if(!strcasecmp(temp, "AC")) {
		    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
		    
		    if(current1 -> ac == NULL) {
			printf("Could not allocate new node.\n");
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    temp = strtok(NULL, "\t ");
			
		    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
		        printf("Expecting more inputs at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1);
		    }
		       			
		    current1 -> ac -> mag = invalid_number_checker(temp);	    
		    
		    if(current1 -> ac -> mag < 0) {
		        printf("Magnitude must be greater than -1 at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1);
		    }
		    
		    temp = strtok(NULL, "\t ");
			
		    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
	                printf("Expecting more inputs at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1);
		    }
			
		    current1 -> ac -> phase = invalid_number_checker(temp);
		    
		    temp = strtok(NULL, "\t \n(,");	

		    if(temp == NULL || temp[0] == 13) {
		        current1 -> transient = NULL;
		    }
		    else {
		      
			current1 -> transient = (transientComponent*) malloc(sizeof(transientComponent));
			
			if(current1 -> transient == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			if(!strcasecmp(temp, "EXP")) {
			    current1 -> transient -> impulse = EXP;
			    
			    current1 -> transient -> exp = (ExpImpulse*) malloc(sizeof(ExpImpulse));
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
					    
			    current1 -> transient -> exp -> i1 = invalid_number_checker(temp);
			    
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
			    
			    current1 -> transient -> exp -> i2 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> exp -> td1 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> exp -> tc1 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
		    
			    current1 -> transient -> exp -> td2 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> exp -> tc2 = invalid_number_checker(temp);
			    
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
			    current1 -> transient -> sin = (SinImpulse*) malloc(sizeof(SinImpulse));
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
			    
			    current1 -> transient -> sin -> i1 = invalid_number_checker(temp);
			    
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
			    
			    current1 -> transient -> sin -> ia = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> sin -> fr = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> sin -> td = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
		    
			    current1 -> transient -> sin -> df = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> sin -> ph = invalid_number_checker(temp);
			    
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
			    current1 -> transient -> pulse = (PulseImpulse*) malloc(sizeof(PulseImpulse));
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
			    
			    current1 -> transient -> pulse -> i1 = invalid_number_checker(temp);
			    
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
			    
			    current1 -> transient -> pulse -> i2 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> td = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> tr = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
		    
			    current1 -> transient -> pulse -> tf = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> pw = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> per = invalid_number_checker(temp);
			    
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
			    current1 -> transient -> pwl = (PWLImpulse*) malloc(sizeof(PWLImpulse));
			    
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
				
				current1 -> transient -> pwl -> t[j] = invalid_number_checker(temp);
				
				temp = strtok(NULL, "\t ),");
				
				if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				    printf("Expecting more inputs at line %d.\n", counter);
				    printf("Terminating.\n");
				    exit(-1);
				}
				
				current1 -> transient -> pwl -> i[j] = invalid_number_checker(temp);
							
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
			    
			    for(j = 0; j < current1 -> transient -> pwl -> length - 1; j++) {
				  if( current1 -> transient -> pwl -> t[j] >  current1 -> transient -> pwl -> t[j + 1]) {
				      printf("The time/value pairs must be in time ascending order, at line %d.\n", counter);
				      printf("Terminating.\n");
				      exit(-1);
				  }
			    }
			}
			else {
			    printf("Cannot parse line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
		    }		        
		}
		else {
		  
		    current1 -> transient = (transientComponent*) malloc(sizeof(transientComponent));
		    
		    if(current1 -> transient == NULL) {
			printf("Could not allocate new node.\n");
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    if(!strcasecmp(temp, "EXP")) {
			current1 -> transient -> impulse = EXP;
			
			current1 -> transient -> exp = (ExpImpulse*) malloc(sizeof(ExpImpulse));
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
		       			
			current1 -> transient -> exp -> i1 = invalid_number_checker(temp);
			
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
			
			current1 -> transient -> exp -> i2 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> td1 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc1 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> exp -> td2 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc2 = invalid_number_checker(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	

			if(temp == NULL || temp[0] == 13) {
		            current1 -> ac = NULL;
		        }		
			else if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			}
		    }
		    else if(!strcasecmp(temp, "SIN")) {
			current1 -> transient -> impulse = SIN;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = (SinImpulse*) malloc(sizeof(SinImpulse));
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
			
			current1 -> transient -> sin -> i1 = invalid_number_checker(temp);
			
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
			
			current1 -> transient -> sin -> ia = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> fr = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> td = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> sin -> df = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> ph = invalid_number_checker(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
		   
			if(temp == NULL || temp[0] == 13) {
		            current1 -> ac = NULL;
		        }		
			else if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			}
		    }
		    else if(!strcasecmp(temp, "PULSE")) {
			current1 -> transient -> impulse = PULSE;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = (PulseImpulse*) malloc(sizeof(PulseImpulse));
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
			
			current1 -> transient -> pulse -> i1 = invalid_number_checker(temp);
			
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
			
			current1 -> transient -> pulse -> i2 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> td = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> tr = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> pulse -> tf = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> pw = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> per = invalid_number_checker(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
		    
			if(temp == NULL || temp[0] == 13) {
		            current1 -> ac = NULL;
		        }		
			else if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			} 
		    }
		    else if(!strcasecmp(temp, "PWL")) {
			current1 -> transient -> impulse = PWL;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = (PWLImpulse*) malloc(sizeof(PWLImpulse));
			
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
			    if(!strcasecmp(temp, "AC")) {
			        break;
			    }

			
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
			    
			    current1 -> transient -> pwl -> t[j] = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pwl -> i[j] = invalid_number_checker(temp);
			    			    
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
			
			for(j = 0; j < current1 -> transient -> pwl -> length - 1; j++) {
			    if( current1 -> transient -> pwl -> t[j] >  current1 -> transient -> pwl -> t[j + 1]) {
				   printf("The time/value pairs must be in time ascending order, at line %d.\n", counter);
				   printf("Terminating.\n");
				   exit(-1);
			    }
			}
			
			if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
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
		    non_zeroes_Gcomplex += 2;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_G += 2;
		    non_zeroes_Gcomplex += 2;
		}
		
		break;
		
	    /* current source */	
	    /* current source */	
	    case 'i':
	    case 'I':
	    
		current1 = (twoTerminalsElement*) malloc(sizeof(twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		circuit_simulation.group1_elements++;
	      
		current1 -> type = CURRENT_SOURCE;
                current1 -> isG2 = 0;
	    	     
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++circuit_simulation.number_of_elements[CURRENT_SOURCE];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = insert_node(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = insert_node(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = invalid_number_checker(temp);
		
		temp = strtok(NULL, "\t \n(,");
		
		if(temp == NULL || temp[0] == 13) {
		    current1 -> transient = NULL;
		    current1 -> ac = NULL;
		}
		else if(!strcasecmp(temp, "AC")) {
		    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
		    
		    if(current1 -> ac == NULL) {
			printf("Could not allocate new node.\n");
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    temp = strtok(NULL, "\t ");
			
		    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
		        printf("Expecting more inputs at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1);
		    }
		       			
		    current1 -> ac -> mag = invalid_number_checker(temp);
		    
		    if(current1 -> ac -> mag < 0) {
		        printf("Magnitude must be greater than -1 at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1);
		    }
		    
		    temp = strtok(NULL, "\t ");
			
		    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
	                printf("Expecting more inputs at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1);
		    }
			
		    current1 -> ac -> phase = invalid_number_checker(temp);
		    
		    temp = strtok(NULL, "\t \n(,");	

		    if(temp == NULL || temp[0] == 13) {
		        current1 -> transient = NULL;
		    }
		    else {
		      
			current1 -> transient = (transientComponent*) malloc(sizeof(transientComponent));
			
			if(current1 -> transient == NULL) {
			    printf("Could not allocate new node.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			if(!strcasecmp(temp, "EXP")) {
			    current1 -> transient -> impulse = EXP;
			    
			    current1 -> transient -> exp = (ExpImpulse*) malloc(sizeof(ExpImpulse));
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
			    
			    current1 -> transient -> exp -> i1 = invalid_number_checker(temp);
			    
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
			    
			    current1 -> transient -> exp -> i2 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> exp -> td1 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> exp -> tc1 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
		    
			    current1 -> transient -> exp -> td2 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> exp -> tc2 = invalid_number_checker(temp);
			    
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
			    current1 -> transient -> sin = (SinImpulse*) malloc(sizeof(SinImpulse));
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
			    
			    current1 -> transient -> sin -> i1 = invalid_number_checker(temp);
			    
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
			    
			    current1 -> transient -> sin -> ia = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> sin -> fr = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> sin -> td = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
		    
			    current1 -> transient -> sin -> df = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> sin -> ph = invalid_number_checker(temp);
			    
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
			    current1 -> transient -> pulse = (PulseImpulse*) malloc(sizeof(PulseImpulse));
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
			    
			    current1 -> transient -> pulse -> i1 = invalid_number_checker(temp);
			    
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
			    
			    current1 -> transient -> pulse -> i2 = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> td = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> tr = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
		    
			    current1 -> transient -> pulse -> tf = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> pw = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pulse -> per = invalid_number_checker(temp);
			    
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
			    current1 -> transient -> pwl = (PWLImpulse*) malloc(sizeof(PWLImpulse));
			    
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
				
				current1 -> transient -> pwl -> t[j] = invalid_number_checker(temp);
				
				temp = strtok(NULL, "\t ),");
				
				if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				    printf("Expecting more inputs at line %d.\n", counter);
				    printf("Terminating.\n");
				    exit(-1);
				}
				
				current1 -> transient -> pwl -> i[j] = invalid_number_checker(temp);
							
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
			    
			    for(j = 0; j < current1 -> transient -> pwl -> length - 1; j++) {
				  if( current1 -> transient -> pwl -> t[j] >  current1 -> transient -> pwl -> t[j + 1]) {
				      printf("The time/value pairs must be in time ascending order, at line %d.\n", counter);
				      printf("Terminating.\n");
				      exit(-1);
				  }
			    }
			}
			else {
			    printf("Cannot parse line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
    
		    }
		}
		else {
		  
		    current1 -> transient = (transientComponent*) malloc(sizeof(transientComponent));
		    
		    if(current1 -> transient == NULL) {
			printf("Could not allocate new node.\n");
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    if(!strcasecmp(temp, "EXP")) {
			current1 -> transient -> impulse = EXP;
			
			current1 -> transient -> exp = (ExpImpulse*) malloc(sizeof(ExpImpulse));
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
			
			current1 -> transient -> exp -> i1 = invalid_number_checker(temp);
			
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
			
			current1 -> transient -> exp -> i2 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> td1 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc1 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> exp -> td2 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> exp -> tc2 = invalid_number_checker(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
			if(temp == NULL || temp[0] == 13) {
		            current1 -> ac = NULL;
		        }		
			else if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			}
		    }
		    else if(!strcasecmp(temp, "SIN")) {
			current1 -> transient -> impulse = SIN;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = (SinImpulse*) malloc(sizeof(SinImpulse));
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
			
			current1 -> transient -> sin -> i1 = invalid_number_checker(temp);
			
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
			
			current1 -> transient -> sin -> ia = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> fr = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> td = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> sin -> df = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> sin -> ph = invalid_number_checker(temp);
			
			current1 -> transient -> vals = NULL;
			
					
			temp = strtok(NULL, "\t \n),");
	
			if(temp == NULL || temp[0] == 13) {
		            current1 -> ac = NULL;
		        }		
			else if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			}
		    }
		    else if(!strcasecmp(temp, "PULSE")) {
			current1 -> transient -> impulse = PULSE;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = (PulseImpulse*) malloc(sizeof(PulseImpulse));
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
			
			current1 -> transient -> pulse -> i1 = invalid_number_checker(temp);
			
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
			
			current1 -> transient -> pulse -> i2 = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> td = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> tr = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
		
			current1 -> transient -> pulse -> tf = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> pw = invalid_number_checker(temp);
			
			temp = strtok(NULL, "\t ),");
			
			if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
			    printf("Expecting more inputs at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
		        }
			
			current1 -> transient -> pulse -> per = invalid_number_checker(temp);
			
			current1 -> transient -> vals = NULL;
					
			temp = strtok(NULL, "\t \n),");
	
			if(temp == NULL || temp[0] == 13) {
		            current1 -> ac = NULL;
		        }		
			else if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			}
		    }
		    else if(!strcasecmp(temp, "PWL")) {
			current1 -> transient -> impulse = PWL;
			
			current1 -> transient -> exp = NULL;
			current1 -> transient -> sin = NULL;
			current1 -> transient -> pulse = NULL;
			current1 -> transient -> pwl = (PWLImpulse*) malloc(sizeof(PWLImpulse));
			
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
			  
			    if(!strcasecmp(temp, "AC")) {
			        break;
			    }
			
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
			    
			    current1 -> transient -> pwl -> t[j] = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ),");
			    
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    current1 -> transient -> pwl -> i[j] = invalid_number_checker(temp);
			    			    
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
			
			for(j = 0; j < current1 -> transient -> pwl -> length - 1; j++) {
			    if( current1 -> transient -> pwl -> t[j] >  current1 -> transient -> pwl -> t[j + 1]) {
				   printf("The time/value pairs must be in time ascending order, at line %d.\n", counter);
				   printf("Terminating.\n");
				   exit(-1);
			    }
			}
			
			if(!strcasecmp(temp, "AC")) {
			    current1 -> ac = (acComponent*) malloc(sizeof(acComponent));
			    
			    if(current1 -> ac == NULL) {
				printf("Could not allocate new node.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
						
			    current1 -> ac -> mag = invalid_number_checker(temp);
			    
			    if(current1 -> ac -> mag < 0) {
				printf("Magnitude must be greater than -1 at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    temp = strtok(NULL, "\t ");
				
			    if(temp == NULL || temp[0] == 13 || temp[0] == 10) {
				printf("Expecting more inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
				
			    current1 -> ac -> phase = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");	
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
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
	    
		current1 = (twoTerminalsElement*) malloc(sizeof(twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		circuit_simulation.group1_elements++;
	      
		current1 -> type = RESISTANCE;
                current1 -> isG2 = 0;
	    
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++circuit_simulation.number_of_elements[RESISTANCE];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = insert_node(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = insert_node(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = invalid_number_checker(temp);
		
		temp = strtok(NULL, "\t \n");

		if(temp != NULL && !strcasecmp(temp, "G2")) {
                   current1 -> isG2 = 1;
                   circuit_simulation.group2_elements++;
		}
		else if(temp != NULL && temp[0] != 13) {
		    printf("Too many inputs at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		
		current1 -> transient = NULL;
		
		current1 -> ac = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
	            non_zeroes_G++;
		    non_zeroes_Gcomplex++;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_G++;
		    non_zeroes_Gcomplex++;
		}
		
		if(current1 -> positive_terminal && current1 -> negative_terminal) {
		    non_zeroes_G += 2;
		    non_zeroes_Gcomplex += 2;
		}

                if(current1 -> isG2) {
                   if(current1 -> positive_terminal) {
	              non_zeroes_G+=2;
		      non_zeroes_Gcomplex+=2;
		   }

                   if(current1 -> negative_terminal) {
			non_zeroes_G+=2;
			non_zeroes_Gcomplex+=2;
		   }

                }
		
		break;
		
	    /* capacitor */	
	    case 'c':
	    case 'C':
	    
		current1 = (twoTerminalsElement*) malloc(sizeof(twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		circuit_simulation.group1_elements++;
	      
		current1 -> type = CAPACITOR;
                current1 -> isG2 = 0;
	    
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++circuit_simulation.number_of_elements[CAPACITOR];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = insert_node(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = insert_node(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = invalid_number_checker(temp);

		temp = strtok(NULL, "\t \n");
		
		
		if(temp != NULL && !strcasecmp(temp, "G2")) {
                   current1 -> isG2 = 1;
                   circuit_simulation.group2_elements++;
		}
		else if(temp != NULL && temp[0] != 13) {
		    printf("Too many inputs at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		current1 -> transient = NULL;
		
		current1 -> ac = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
	            non_zeroes_C++;
		    non_zeroes_Gcomplex++;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_C++;
		    non_zeroes_Gcomplex++;
		}
		
		if(current1 -> positive_terminal && current1 -> negative_terminal) {
		    non_zeroes_C += 2;
		    non_zeroes_Gcomplex += 2;
		}

                if(current1 -> isG2) {
			non_zeroes_G++;

                        if(current1 -> positive_terminal) {
			    non_zeroes_C++;
			}
		
			if(current1 -> negative_terminal) {
			    non_zeroes_C++;
			}

                        non_zeroes_Gcomplex++;

                }
		
		break;
		
	    /* inductor*/	
	    case 'l':
	    case 'L':
	    
		current1 = (twoTerminalsElement*) malloc(sizeof(twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		circuit_simulation.group2_elements++;
	      
		current1 -> type = INDUCTOR;
                current1 -> isG2 = 1;
	    
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++circuit_simulation.number_of_elements[INDUCTOR];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = insert_node(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = insert_node(temp);	
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = invalid_number_checker(temp);
		
		temp = strtok(NULL, "\t \n");
		
		if(temp != NULL && temp[0] != 13) {
		    printf("Too many inputs at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		current1 -> transient = NULL;
		
		current1 -> ac = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
		    non_zeroes_G++;
		    non_zeroes_Gcomplex++;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_G++;
		    non_zeroes_Gcomplex++;
		}
		
		 non_zeroes_G++;
		 non_zeroes_Gcomplex++;
		
		break;


             /* vcvs*/	
	    case 'e':
	    case 'E':
	    
		current1 = (twoTerminalsElement*) malloc(sizeof(twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		circuit_simulation.group2_elements++;
	      
		current1 -> type = VOLTAGE_CONTROLLED_VOLTAGE_SOURCE;
                current1 -> isG2 = 1;
	    
		temp = strtok(line + i + 1, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		strcpy(current1 -> string_name, temp);

		current1 -> name = ++circuit_simulation.number_of_elements[VOLTAGE_CONTROLLED_VOLTAGE_SOURCE];
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> in_positive_terminal = insert_node(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> in_negative_terminal = insert_node(temp);

                temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Positive terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	 	      
		current1 -> positive_terminal = insert_node(temp);
		
		temp = strtok(NULL, "\t ");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Negative terminal name not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      	      
		current1 -> negative_terminal = insert_node(temp);		
		
		temp = strtok(NULL, "\t \n");
		
		if(temp == NULL || temp[0] == 13) {
		    printf("Element value not found at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> value = invalid_number_checker(temp);
		
		temp = strtok(NULL, "\t \n");
		
		if(temp != NULL && temp[0] != 13) {
		    printf("Too many inputs at line %d.\n", counter);
	            printf("Terminating.\n");
	            exit(-1);
		}
		
		current1 -> transient = NULL;
		
		current1 -> ac = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
		
		if(current1 -> positive_terminal) {
		    non_zeroes_G += 2;
		    non_zeroes_Gcomplex += 2;
		}
		
		if(current1 -> negative_terminal) {
		    non_zeroes_G += 2;
		    non_zeroes_Gcomplex += 2;
		}
		
		if(current1 -> in_negative_terminal) {
		    non_zeroes_C++;
		    non_zeroes_Gcomplex++;
		}

                if(current1 -> in_positive_terminal) {
		    non_zeroes_C++;
		    non_zeroes_Gcomplex++;
		}
		
		break;
		
	    /* diode */	
	    case 'd':
	    case 'D':
	    
		current1 = (twoTerminalsElement*) malloc(sizeof(twoTerminalsElement));
		
		if(current1 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
	      
		current1 -> type = DIODE;
                current1 -> isG2 = 0;
	   	      
		strcpy(current1 -> string_name, strtok(line + i + 1, "\t "));
		
		current1 -> name = ++circuit_simulation.number_of_elements[DIODE];
	   	      
		current1 -> positive_terminal = insert_node(strtok(NULL, "\t "));
	      
		current1 -> negative_terminal = insert_node(strtok(NULL, "\t "));
		      
		strtok(NULL, "\t ");  //MODEL NAME(ignored)	    
	      
		temp = strtok(NULL, "\t \n");
		//the field area is optional
		if(temp == NULL) {
		    current1 -> value = 1;
		}
		else {
		    current1 -> value = invalid_number_checker(temp); 
		}
		
		current1 -> transient = NULL;
		
		current1 -> ac = NULL;
	      
		current1 -> next = NULL;
		
		insertTwoTerminalsElement(current1);
	      
		break;
		
	    /* bjt transistor */	
	    case 'q':
	    case 'Q':
	      
		current2 = (threeTerminalsElement*) malloc(sizeof(threeTerminalsElement));
		
		if(current2 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		  
		current2 -> type = BJT;
		 	      
		strcpy(current2 -> string_name, strtok(line + i + 1, "\t "));
		
		current2 -> name = ++circuit_simulation.number_of_elements[BJT];
			      
		current2 -> c_terminal = insert_node(strtok(NULL, "\t "));
			      
		current2 -> b_terminal = insert_node(strtok(NULL, "\t "));
			      
		current2 -> e_terminal = insert_node(strtok(NULL, "\t "));
		
		strtok(NULL, "\t ");  //MODEL NAME(ignored)
		
		temp = strtok(NULL, "\t \n");
		//the field area is optional
		if(temp == NULL) {
		    current2 -> value = 1;
		}
		else {
		    current2 -> value = invalid_number_checker(temp); 
		}
	      
		current2 -> next = NULL;
		
		insertThreeTerminalsElement(current2);
	      
		break;
		
	    /* mos transistor */	
	    case 'm':
	    case 'M':
	      
		current3 = (fourTerminalsElement*) malloc(sizeof(fourTerminalsElement));
		
		if(current3 == NULL) {
		    printf("Could not allocate new node.\n");
	            printf("Terminating.\n");
	            exit(-1);
		}
		  
		current3 -> type = MOS;
		  	      
		strcpy(current3 -> string_name, strtok(line + i + 1, "\t "));
		
		current3 -> name = ++circuit_simulation.number_of_elements[MOS];
	      
		current3 -> d_terminal = insert_node(strtok(NULL, "\t "));
	      
		current3 -> g_terminal = insert_node(strtok(NULL, "\t "));
	      
		current3 -> s_terminal = insert_node(strtok(NULL, "\t "));
	      
		current3 -> b_terminal = insert_node(strtok(NULL, "\t "));
		
		strtok(NULL, "\t ");  //MODEL NAME(ignored)
		
		current3 -> length = invalid_number_checker(strtok(NULL, "\t lL="));
		
		current3 -> width = invalid_number_checker(strtok(NULL, "\t wW=\n"));
	      
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
			    circuit_simulation.matrix_type = SPD;
			    printf("Options: Symmetric & Positive Matrices Activated.\n");
			}
			else if(!strcasecmp(temp, "ITER")) {
			    circuit_simulation.method = ITERATIVE;
			    printf("Options: Iterative Methods Activated.\n");
			}
			else if(!strcasecmp(temp, "ITOL")) {
			    temp = strtok(NULL, "\t \n=");
			    if(temp == NULL) {
			        printf("Cannot parse line %d.\n", counter);
			        printf("Terminating.\n");
			        exit(-1); 
			    }
			    circuit_simulation.iterative_methods.itol = invalid_number_checker(temp);
			    printf("Options: Iterative Threshold set to %.8e .\n", circuit_simulation.iterative_methods.itol);
			}
			else if(!strcasecmp(temp, "SPARSE")) {
			    circuit_simulation.matrix_sparsity = SPARSE;
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
				circuit_simulation.diff_method = TRAPEZOIDAL;
				printf("Options: Trapezoidal Method Activated.\n");
			      
			    }
			    else if(!strcasecmp(temp, "BE")) {
				circuit_simulation.diff_method = BACKWARD_EULER;
				printf("Options: Backward Euler Method Activated.\n");
			    }
			    else {
				printf("Cannot parse line %d.\n", counter);
			        printf("Terminating.\n");
			        exit(-1); 
			    }
			}
			else if(!strcasecmp(temp, "GNUPLOTLP")) {
			    circuit_simulation.gnuplot = LINES_POINTS;
			    printf("Options: GNU plot Activated (Lines and Points).\n");
			}
			else if(!strcasecmp(temp, "GNUPLOTL")) {
			    circuit_simulation.gnuplot = LINES;
			    printf("Options: GNU plot Activated (Lines).\n");
			}
			else if(!strcasecmp(temp, "GRAPH")) {
			    circuit_simulation.dot_graph = DOT_GRAPH_ON;
			    printf("Options: Dot graph Activated.\n");
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
		    
		    if(circuit_simulation.number_of_sweeps == 0)
		    {
		        circuit_simulation.dc_sweep_settings = (dcSweep *)malloc(sizeof(dcSweep));		
			
			if(circuit_simulation.dc_sweep_settings == NULL) {
			    printf("Could not allocate memory.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep = SWEEP_OFF;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start = 0;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end = 0;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step = 0;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index1 = -1;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index2 = -1;
			
		    }
		    else {
		        circuit_simulation.dc_sweep_settings = (dcSweep *)realloc(circuit_simulation.dc_sweep_settings, (circuit_simulation.number_of_sweeps + 1) * sizeof(dcSweep));
			
			if(circuit_simulation.dc_sweep_settings == NULL) {
			    printf("Could not allocate memory.\n");
			    printf("Terminating.\n");
			    exit(-1);
			}
			
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep = SWEEP_OFF;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start = 0;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end = 0;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step = 0;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index1 = -1;
			(circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index2 = -1;
			
		    }
		    
		    switch (temp[0]) {
		        case 'V':
		        case 'v':
			    temp++;
			    
			    if(temp[0] == '\0') {
			        temp = strtok(NULL, "\t ");
			    }
			    
			    for(k = 0; k < strlen(temp); k++) {
				if(temp[k] == 13) {
				    temp[k] = '\0'; 
				}
			    }
			    
			    for(current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {
			        if(current1 -> type == VOLTAGE_SOURCE || current1 -> type == INDUCTOR) {
				    temp_group2++;
				}
			        if(current1 -> type == VOLTAGE_SOURCE && !strcasecmp(temp, current1 -> string_name)) {
                                    break;
				}
			    }
			    
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index1 = -1;
                           (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index2 = -1;
			    
			    if(current1 == NULL) {
			        printf("\n\nThe element, V%s does not exist, at line %d.\n", temp, counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep = SWEEP_VOLTAGE_SOURCE;
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index1 = temp_group2 - 1;
			    
			    temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep start value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep end value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                           (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep step value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                           (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start == (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end) {
			        printf("\n\nThe start and stop value must no be equal, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step == 0) {
			        printf("\n\nThe sweep step must not be 0, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start < (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end && (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step < 0) {
			        printf("\n\nThe sweep step must be greater than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start > (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end && (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step > 0) {
			        printf("\n\nThe sweep step must be less than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->sweep_name[0] = '\0';
			    strcat((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->sweep_name, "V ");
			    strcat((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->sweep_name, current1 -> string_name);
	
			    break;
			  
			case 'I':
			case 'i':
			    temp++;
			    
			    if(temp[0] == '\0') {
			        temp = strtok(NULL, "\t ");
			    }
			    
			    for(k = 0; k < strlen(temp); k++) {
				if(temp[k] == 13) {
				    temp[k] = '\0'; 
				}
			    }
			    
			    for(current1 = head_twoTerminalsElement_list; head_twoTerminalsElement_list != NULL && current1 != NULL; current1 = current1 -> next) {	  
			        if(current1 -> type == CURRENT_SOURCE && !strcasecmp(temp, current1 -> string_name)) {
                                    break;
				}
			    }
			    
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index1 = -1;
                           (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index2 = -1;
			    
			    if(current1 == NULL) {
			        printf("\n\nThe element, I%s does not exist, at line %d.\n", temp, counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep = SWEEP_CURRENT_SOURCE;
			    
			    if(current1 -> positive_terminal) {
		                 (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index1 = current1 -> positive_terminal - 1;
		            }
		
		            if(current1 -> negative_terminal) {
		                 (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_index2 = current1 -> negative_terminal - 1;
		            }
		            
		            temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep start value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t ");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep end value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                           (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    if(temp == NULL || temp[0] == 13) {
				printf("DC sweep step value not found at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }
                           (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step = invalid_number_checker(temp);
			    
			    temp = strtok(NULL, "\t \n");
			    
			    if(temp != NULL && temp[0] != 13) {
				printf("Too many inputs at line %d.\n", counter);
				printf("Terminating.\n");
				exit(-1);
			    }			    
			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start == (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end) {
			        printf("\n\nThe start and stop value must not be equal, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step == 0) {
			        printf("\n\nThe sweep step must not be 0, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }			    
			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start < (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end && (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step < 0) {
			        printf("\n\nThe sweep step must be greater than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    if((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_start > (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_end && (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->dc_sweep_step > 0) {
			        printf("\n\nThe sweep step must be less than zero, at line %d.\n", counter);
		                printf("Terminating.\n");
		                exit(-1); 
			    }
			    
			    (circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->sweep_name[0] = '\0';
			    strcat((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->sweep_name, "I ");
			    strcat((circuit_simulation.dc_sweep_settings + circuit_simulation.number_of_sweeps)->sweep_name, current1 -> string_name);
               
			    break;			    
			  
			default:
			     printf("Cannot parse line %d.\n", counter);
		             printf("Terminating.\n");
		             exit(-1);
		      
		    }
		    
		    circuit_simulation.number_of_sweeps++;
		}
		else if(!strcasecmp(temp, "PLOT") || !strcasecmp(temp, "PRINT")) {
		  
		    temp = strtok(NULL, "\t ");
		    
		    for(k = 0; k < strlen(temp); k++) {
			if(temp[k] == 13) {
			    temp[k] = '\0'; 
			}
		    }
		    
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
			    
			    (circuit_simulation.plot_settings + temp3) -> dc_sweep_plot = ENABLED;
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
			    
			    (circuit_simulation.plot_settings + temp3) -> transient_plot = ENABLED;
			} while(1);
		    }
		    else if(!strcasecmp(temp, "AC")) {
		      
		        temp = strtok(NULL, "\t \n");
			
			if(!strcasecmp(temp, "LINEAR")) {
			    circuit_simulation.ac_plot_scale = LINEAR;
			}
			else if(!strcasecmp(temp, "DB")) {
			    circuit_simulation.ac_plot_scale = DB;
			}
			else {
			    printf("Expecting plot scale mode (LINEAR or DB), at line %d.\n", counter);
			    printf("Terminating.\n");
			    exit(-1);
			}
		        
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
			    
			    (circuit_simulation.plot_settings + temp3) -> ac_plot = ENABLED;
			} while(1);
		    }
		    else {
			printf("Expecting plot mode (DC or TRAN or AC), at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }  
		}
		else if(!strcasecmp(temp, "TRAN")) {
		  
		     if(circuit_simulation.number_of_transient_analysis == 0)
		     {
			    circuit_simulation.transient_analysis_settings = (transientAnalysis *)malloc(sizeof(transientAnalysis));		
			    
			    if(circuit_simulation.transient_analysis_settings == NULL) {
				printf("Could not allocate memory.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->time_step = 0;
			    (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->fin_time = 0;
			    
	            }
		    else {
			    circuit_simulation.transient_analysis_settings = (transientAnalysis  *)realloc(circuit_simulation.transient_analysis_settings, (circuit_simulation.number_of_transient_analysis + 1) * sizeof(transientAnalysis ));
			    
			    if(circuit_simulation.transient_analysis_settings == NULL) {
				printf("Could not allocate memory.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->time_step = 0;
			    (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->fin_time = 0;
		    }
		    
		    temp = strtok(NULL, "\t \n");
		    if(temp == NULL || temp[0] == 13) {
			printf("Transient time step value not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
		    (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->time_step = invalid_number_checker(temp);
		    
		    temp = strtok(NULL, "\t \n");
		    if(temp == NULL || temp[0] == 13) {
			printf("Transient finish time value not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
                    (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->fin_time = invalid_number_checker(temp);
		    
		    temp = strtok(NULL, "\t \n");
			    
		    if(temp != NULL && temp[0] != 13) {
			printf("Too many inputs at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }		
		    
		    if( (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->time_step <= 0) {
			 printf("\n\nThe time step must be greater than 0, at line %d.\n", counter);
		         printf("Terminating.\n");
		         exit(-1); 
		    }
		    
		    if( (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->fin_time <= 0) {
			 printf("\n\nThe finish time must be greater than 0, at line %d.\n", counter);
		         printf("Terminating.\n");
		         exit(-1); 
		    }
		    
		    if( (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->time_step >=  (circuit_simulation.transient_analysis_settings + circuit_simulation.number_of_transient_analysis)->fin_time) {
			printf("\n\nThe time step must lower than finish time, at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1); 
		    }
		    
		    circuit_simulation.number_of_transient_analysis++;
	
		}
		else if(!strcasecmp(temp, "AC")) {
		  
		     if(circuit_simulation.number_of_ac_analysis == 0)
		     {
			    circuit_simulation.ac_analysis_settings = (acAnalysis *)malloc(sizeof(acAnalysis));		
			    
			    if(circuit_simulation.ac_analysis_settings == NULL) {
				printf("Could not allocate memory.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->scale = LIN;
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->points = 0;
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->start_freq = 0;
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->end_freq = 0;	
			    
	            }
		    else {
			    circuit_simulation.ac_analysis_settings = (acAnalysis *)realloc(circuit_simulation.ac_analysis_settings, (circuit_simulation.number_of_ac_analysis + 1) * sizeof(acAnalysis));
			    
			    if(circuit_simulation.ac_analysis_settings == NULL) {
				printf("Could not allocate memory.\n");
				printf("Terminating.\n");
				exit(-1);
			    }
			    
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->scale = LIN;
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->points = 0;
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->start_freq = 0;
			    (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->end_freq = 0;
		    
		    }
		    
		    temp = strtok(NULL, "\t \n");
		    if(temp == NULL || temp[0] == 13) {
			printf("AC analysis scale not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    for(k = 0; k < strlen(temp); k++) {
			if(temp[k] == 13) {
			    temp[k] = '\0'; 
			}
		    }
		    
		    if(!strcasecmp(temp, "LIN")) {
		        (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->scale = LIN;
		    }
		    else if(!strcasecmp(temp, "LOG")) {
		        (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->scale = LOG;
		    }
		    else {
		        printf("Invalid scale found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    temp = strtok(NULL, "\t \n");
		    if(temp == NULL || temp[0] == 13) {
			printf("Points value not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
                   (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->points = invalid_number_checker(temp);
		    
		    temp = strtok(NULL, "\t \n");
		    if(temp == NULL || temp[0] == 13) {
			printf("Starting frequency not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
                   (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->start_freq = invalid_number_checker(temp);
		    
		    temp = strtok(NULL, "\t \n");
		    if(temp == NULL || temp[0] == 13) {
			printf("Ending frequency not found at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
                   (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->end_freq = invalid_number_checker(temp);
		    
		    temp = strtok(NULL, "\t \n");
			    
		    if(temp != NULL && temp[0] != 13) {
			printf("Too many inputs at line %d.\n", counter);
			printf("Terminating.\n");
			exit(-1);
		    }
		    
		    if( (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->points <= 0) {
			 printf("\n\nPoints must be greater than 0, at line %d.\n", counter);
		         printf("Terminating.\n");
		         exit(-1); 
		    }
		    
		    if( (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->start_freq < 0) {
			 printf("\n\nStarting frequency must be greater than -1, at line %d.\n", counter);
		         printf("Terminating.\n");
		         exit(-1); 
		    }
		    
		    if( (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->end_freq < 0) {
			 printf("\n\nEnding frequency must be greater than -1, at line %d.\n", counter);
		         printf("Terminating.\n");
		         exit(-1); 
		    }
		   		    
		    if( (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->end_freq <=  (circuit_simulation.ac_analysis_settings + circuit_simulation.number_of_ac_analysis)->start_freq) {
			printf("\n\nThe starting frequency must be lower than the ending frequency, at line %d.\n", counter);
		        printf("Terminating.\n");
		        exit(-1); 
		    }
		    
		    circuit_simulation.number_of_ac_analysis++;
	
		}
		else if(!strcasecmp(temp, "OP")) {
		     circuit_simulation.dc_analysis_settings.operating_point = ENABLED; // for printing currents of the elements at dc analysis
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

    printf("             Nodes: %7d\n", circuit_simulation.number_of_nodes + 1);
    
    if(circuit_simulation.number_of_elements[VOLTAGE_SOURCE]) {
	printf("   Voltage Sources: %7d\n", circuit_simulation.number_of_elements[VOLTAGE_SOURCE]);
    }
    
    if(circuit_simulation.number_of_elements[CURRENT_SOURCE]) {
	printf("   Current Sources: %7d\n", circuit_simulation.number_of_elements[CURRENT_SOURCE]);
    }
    
    if(circuit_simulation.number_of_elements[RESISTANCE]) {
	printf("       Resistances: %7d\n", circuit_simulation.number_of_elements[RESISTANCE]);
    }
    
    if(circuit_simulation.number_of_elements[CAPACITOR]) {
	printf("        Capacitors: %7d\n", circuit_simulation.number_of_elements[CAPACITOR]);
    }
    
    if(circuit_simulation.number_of_elements[INDUCTOR]) {
	printf("         Inductors: %7d\n", circuit_simulation.number_of_elements[INDUCTOR]);
    }

    if(circuit_simulation.number_of_elements[VOLTAGE_CONTROLLED_VOLTAGE_SOURCE]) {
	printf("VC Voltage Sources: %7d\n", circuit_simulation.number_of_elements[VOLTAGE_CONTROLLED_VOLTAGE_SOURCE]);
    }
    
    if(circuit_simulation.number_of_elements[DIODE]) {
	printf("         Diodes: %7d\n", circuit_simulation.number_of_elements[DIODE]);
    }
    
    if(circuit_simulation.number_of_elements[BJT]) {
	printf("BJT Transistors: %7d\n", circuit_simulation.number_of_elements[BJT]);
    }
    
    if(circuit_simulation.number_of_elements[MOS]) {
	printf("MOS Transistors: %7d\n", circuit_simulation.number_of_elements[MOS]);
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
    
    free_avl_tree();
    
}


void insertTwoTerminalsElement(twoTerminalsElement *current) {
 
    if(head_twoTerminalsElement_list == NULL) {
        head_twoTerminalsElement_list = current;
	last_twoTerminalsElement_list = current;
    }
    else {
        last_twoTerminalsElement_list -> next = current;
	last_twoTerminalsElement_list = current;	
    }
    
}


void insertThreeTerminalsElement(threeTerminalsElement *current) { 
 
    if(head_threeTerminalsElement_list == NULL) {
        head_threeTerminalsElement_list = current;
	last_threeTerminalsElement_list = current;
    }
    else {
        last_threeTerminalsElement_list -> next = current;
	last_threeTerminalsElement_list = current;	
    }
    
}


void insertFourTerminalsElement(fourTerminalsElement *current) {
 
    if(head_fourTerminalsElement_list == NULL) {
        head_fourTerminalsElement_list = current;
	last_fourTerminalsElement_list = current;
    }
    else {
        last_fourTerminalsElement_list -> next = current;
	last_fourTerminalsElement_list = current;	
    }
    
}

double invalid_number_checker(char *string) {
  double temp;
  char *p;

     errno = 0;
     
     temp = strtod(string, &p);
 
     if(errno != 0 || string == p || (*p != '\0' && *p != 13 && *p != 10)) {
         printf("Invalid input. Required number, found string at line %d.\n", counter);
	 printf("Terminating.\n");
	 exit(-1);
     }
     
     return temp;
  
}

