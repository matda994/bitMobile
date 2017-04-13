/*  General-purpose fuzzy inference engine supporting any number of system
 inputs and outputs, membership functions, and rules. Membership functions can
 be any shape defineable by 2 points and 2 slopes--trapezoids, triangles,
 rectanlges, etc. Rules can have any number of antecedents and outputs, and can
 vary from rule to rule. "Min" method is used to compute rule strength, "Max"
 for applying rule strengths, "Center-of-Gravity" for defuzzification.
 */

#ifndef general_FIS_h
#define general_FIS_h


//////////////////////////////////////////////////////////////////////////////////
// DECLARATION OF DATA STRUCTURES FOR LINKED LISTS                              //
//////////////////////////////////////////////////////////////////////////////////

#define MAXNAME 10          /* max number of characters in names           */
#define UPPER_LIMIT 100    /* max number assigned as degree of membership */

extern struct io_type *System_Outputs;
extern struct io_type *System_Inputs;
extern struct rule_type *Rule_Base;


//////////////////////////////////////////////////////////////////////////////////
// DEFINITION OF DATA STRUCTURES FOR LINKED LISTS                               //
//////////////////////////////////////////////////////////////////////////////////

/* io_type structure builds a list of system inputs and a list of system
 outputs. After initialization, these lists are fixed, except for value field
 which is updated on every inference pass. */
struct io_type{
    char name[MAXNAME];        /*  name of system input/output       */
    int value;                 /*  value of system input/output      */
    struct mf_type             /*  list of membership functions for  */
    *membership_functions;   /*     this system input/output       */
    struct io_type *next;      /*  pointer to next input/output      */
};


/* Membership functions are associated with each system input and output. */
struct mf_type{
    char name[MAXNAME]; /* name of membership function (fuzzy set)    */
    int value;          /* degree of membership or output strength    */
    int point1;         /* leftmost x-axis point of mem. function     */
    int point2;         /* rightmost x-axis point of mem. function    */
    int slope1;         /* slope of left side of membership function  */
    int slope2;         /* slope of right side of membership function */
    struct mf_type *next;   /* pointer to next membership function    */
};


/*  Each rule has an if side and a then side. Elements making up if side are
 pointers to antecedent values inside mf_type structure. Elements making up then
 side of rule are pointers to output strength values, also inside mf_type
 structure. Each rule structure contains a pointer to next rule in rule base. */
struct rule_element_type{
    int *value;                /* pointer to antecedent/output strength value */
    struct rule_element_type *next; /* next antecedent/output element in rule */
};
struct rule_type{
    struct rule_element_type *if_side;     /* list of antecedents in rule */
    struct rule_element_type *then_side;   /* list of outputs in rule     */
    struct rule_type *next;                /* next rule in rule base      */
};


//////////////////////////////////////////////////////////////////////////////////
// DECLARATION METHODS THAT PERFORM FUZZY LOGIC CONTROL                         //
//////////////////////////////////////////////////////////////////////////////////

void fuzzification();
void rule_evaluation();
void defuzzification();


//////////////////////////////////////////////////////////////////////////////////
// DECLARATION OF POINTERS TO TOP OF LINKED LISTS                               //
//////////////////////////////////////////////////////////////////////////////////

void setPointers(struct io_type*, struct io_type*, struct rule_type*);


#endif /* general_FIS_h */
