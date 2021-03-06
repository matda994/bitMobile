﻿//
//  main.c
//  fis
//
//  Created by Mathias Dalshagen on 2017-04-08.
//  The code for the FIS was fetched from: http://www.drdobbs.com/cpp/fuzzy-logic-in-c/184408940
//




#include <stdio.h>
#include <string.h>





////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////




/*  General-purpose fuzzy inference engine supporting any number of system
 inputs and outputs, membership functions, and rules. Membership functions can
 be any shape defineable by 2 points and 2 slopes--trapezoids, triangles,
 rectanlges, etc. Rules can have any number of antecedents and outputs, and can
 vary from rule to rule. "Min" method is used to compute rule strength, "Max"
 for applying rule strengths, "Center-of-Gravity" for defuzzification.
 */

#define MAXNAME 10          /* max number of characters in names           */
#define UPPER_LIMIT  255    /* max number assigned as degree of membership */

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






////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////



struct io_type *System_Inputs;
struct io_type *System_Outputs;
struct rule_type *Rule_Base;
struct io_type lateral;
struct io_type longitudinal;
struct io_type pwm;





////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


/* Returns the max value of two integers */
int max(arg1,arg2)
int arg1;
int arg2;
{
    if (arg1>arg2)
    {
        return(arg1);
    }
    else
    {
        return(arg2);
    }
}

/* Returns the min value of two integers */
int min(arg1,arg2)
int arg1;
int arg2;
{
    if (arg1<arg2)
    {
        return(arg1);
    }
    else
    {
        return(arg2);
    }
}

/* Compute Degree of Membership--Degree to which input is a member of mf is
 calculated as follows: 1. Compute delta terms to determine if input is inside
 or outside membership function. 2. If outside, then degree of membership is 0.
 Otherwise, smaller of delta_1 * slope1 and delta_2 * slope2 applies.
 3. Enforce upper limit. */
void compute_degree_of_membership(mf,input)
struct mf_type *mf;
int input;
{
    int delta_1;
    int delta_2;
    delta_1 = input - mf->point1;
    delta_2 = mf->point2 - input;
    if ((delta_1 <= 0) || (delta_2 <= 0))   /* input outside mem. function ?  */
        mf->value = 0;                           /* then degree of membership is 0 */
        else
            mf->value = min( (mf->slope1*delta_1),(mf->slope2*delta_2) );
            mf->value = min(mf->value,UPPER_LIMIT);  /* enforce upper limit */
            }


/* Compute Area of Trapezoid--Each inference pass produces a new set of output
 strengths which affect the areas of trapezoidal membership functions used in
 center-of-gravity defuzzification. Area values must be recalculated with each
 pass. Area of trapezoid is h*(a+b)/2 where h=height=output_strength=mf->value
 b=base=mf->point2-mf->point1 a=top= must be derived from h,b, and slopes1&2 */
int compute_area_of_trapezoid(mf)
struct mf_type *mf;
{
    int run_1;
    int run_2;
    int base;
    int top;
    int area;
    base = mf->point2 - mf->point1;
    run_1 = mf->value/mf->slope1;
    run_2 = mf->value/mf->slope2;
    top = base - run_1 - run_2;
    area = mf->value * ( base + top)/2;
    return(area);
}



/* Fuzzification--Degree of membership value is calculated for each membership
 function of each system input. Values correspond to antecedents in rules. */
void fuzzification()
{
    struct io_type *si;    /* system input pointer        */
    struct mf_type *mf;    /* membership function pointer */
    for(si=System_Inputs; si != NULL; si=si->next)
        for(mf=si->membership_functions; mf != NULL; mf=mf->next)
            if (mf!=NULL) {
                compute_degree_of_membership(mf,si->value);
            }
}
/* Rule Evaluation--Each rule consists of a list of pointers to antecedents
 (if side), list of pointers to outputs (then side), and pointer to next rule
 in rule base. When a rule is evaluated, its antecedents are ANDed together,
 using a minimum function, to form strength of rule. Then strength is applied
 to each of listed rule outputs. If an output has already been assigned a rule
 strength, during current inference pass, a maximum function is used to
 determine which strength should apply. */
void rule_evaluation()
{
    struct rule_type *rule;
    struct rule_element_type *ip;       /* pointer of antecedents  (if-parts)   */
    struct rule_element_type *tp;       /* pointer to consequences (then-parts) */
    int strength;                /* strength of  rule currently being evaluated */
    for(rule=Rule_Base; rule != NULL; rule=rule->next){
        strength = UPPER_LIMIT;                       /* max rule strength allowed */
        /* process if-side of rule to determine strength */
        for(ip=rule->if_side; ip != NULL; ip=ip->next)
            strength = min(strength,*(ip->value));
        /* process then-side of rule to apply strength */
        for(tp=rule->then_side; tp != NULL; tp=tp->next)
            *(tp->value) = max(strength,*(tp->value));
    }
}


/* Defuzzification */
void defuzzification()
{
    struct io_type *so;    /* system output pointer                  */
    struct mf_type *mf;    /* output membership function pointer     */
    int sum_of_products;   /* sum of products of area & centroid */
    int sum_of_areas;     /* sum of shortend trapezoid area          */
    int area;
    int centroid;
    /* compute a defuzzified value for each system output */
    for(so=System_Outputs; so != NULL; so=so->next){
        sum_of_products = 0;
        sum_of_areas = 0;
        for(mf=so->membership_functions; mf != NULL; mf=mf->next){
            area = compute_area_of_trapezoid(mf);
            centroid = mf->point1 + (mf->point2 - mf->point1)/2;
            sum_of_products += area * centroid;
            sum_of_areas += area;
        }
        if (sum_of_areas>0) {
            so->value = sum_of_products/sum_of_areas;   /* weighted average */
        }
        else
        {
            so->value = 0;
        }
    }
}


int outPutToESC(fuzzyOutput)
int fuzzyOutput;
{
	int dESC = 0;
	for (int i = 4 ; i>=-4 ; i--)
	{
		if (fuzzyOutput <= i*100+50 && fuzzyOutput >= i*100-50)
		{
			dESC = i;
		}
	}
	return(dESC);
}


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


void changeValues(lat,lon)
int lat;
int lon;
{
	longitudinal.value = lon;
	lateral.value = lat;
	
	};





void initializeTest(void) {
    
    
    
    struct mf_type faar;
    strcpy(faar.name, "far");
    faar.value = 0;
    faar.point1 = 150;
    faar.point2 = 300;
    faar.slope1 = 2;
    faar.slope2 = 127;
    faar.next = NULL;
    
    struct mf_type detected;
    strcpy(detected.name, "detected");
    detected.value = 0;
    detected.point1 = 60;
    detected.point2 = 240;
    detected.slope1 = 3;
    detected.slope2 = 3;
    detected.next = &faar;
    
    
    struct mf_type close;
    strcpy(close.name, "close");
    close.value = 0;
    close.point1 = 0;
    close.point2 = 150;
    close.slope1 = 127;
    close.slope2 = 2;
    close.next = &detected;
    
    longitudinal.membership_functions = &close;
    
    
    struct mf_type big;
    strcpy(big.name, "big");
    big.value = 0;
    big.point1 = 4;
    big.point2 = 20;
    big.slope1 = 23;
    big.slope2 = 127;
    big.next = NULL;
    
    struct mf_type small;
    strcpy(small.name, "small");
    small.value = 0;
    small.point1 = 0;
    small.point2= 15;
    small.slope1 = 127;
    small.slope2 = 23;
    small.next = &big;
    
    lateral.membership_functions = &small;
    strcpy(lateral.name, "lateral");
    strcpy(longitudinal.name, "longit");
    lateral.next = &longitudinal;
    longitudinal.next = NULL;
    System_Inputs = &lateral;
    
    struct mf_type gas;
    strcpy(gas.name, "gas");
    gas.value = 0;
    gas.point1 = 0;
    gas.point2 = 400;
    gas.slope1 = 1;
    gas.slope2 = 3;
    gas.next = NULL;
    
    struct mf_type steady;
    strcpy(steady.name, "steady");
    steady.value = 0;
    steady.point1 = -255;
    steady.point2 = 255;
    steady.slope1 = 1;
    steady.slope2 = 1;
    steady.next = &gas;
    
    struct mf_type brake;
    strcpy(brake.name, "brake");
    brake.value = 0;
    brake.point1 = -400;
    brake.point2 = 0;
    brake.slope1 = 3;
    brake.slope2 = 1;
    brake.next = &steady;
    
    pwm.membership_functions = &brake;
    System_Outputs = &pwm;
    pwm.next = NULL;
    
    
    
    struct rule_element_type then5;
    then5.value = &brake.value;
    then5.next = NULL;
    
    struct rule_element_type if52;
    if52.value = &big.value;
    if52.next = NULL;
    
    struct rule_element_type if51;
    if51.value = &faar.value;
    if51.next = &if52;
    
    struct rule_type rule5;
    rule5.if_side = &if51;
    rule5.then_side = &then5;
    rule5.next = NULL;
    
    struct rule_element_type then4;
    then4.value = &gas.value;
    then4.next = NULL;
    
    struct rule_element_type if42;
    if42.value = &small.value;
    if42.next = NULL;
    
    struct rule_element_type if41;
    if41.value = &faar.value;
    if41.next = &if42;
    
    struct rule_type rule4;
    rule4.if_side = &if41;
    rule4.then_side = &then4;
    rule4.next = &rule5;
    
    
    struct rule_element_type then3;
    then3.value = &brake.value;
    then3.next = NULL;
    
    struct rule_element_type if32;
    if32.value = &big.value;
    if32.next = NULL;
    
    struct rule_element_type if31;
    if31.value = &detected.value;
    if31.next = &if32;
    
    struct rule_type rule3;
    rule3.if_side = &if31;
    rule3.then_side = &then3;
    rule3.next = &rule4;
    
    
    
    struct rule_element_type if22;
    if22.value = &big.value;
    if22.next = NULL;
    
    struct rule_element_type if21;
    if21.value = &close.value;
    if21.next = &if22;
    
    struct rule_element_type then2;
    then2.value = &steady.value;
    then2.next = NULL;
    
    struct rule_type rule2;
    rule2.if_side = &if21;
    rule2.then_side = &then2;
    rule2.next = &rule3;
    
    
    
    struct rule_element_type if11;
    if11.value = &close.value;
    if11.next = NULL;
    
    struct rule_element_type then1;
    then1.value = &brake.value;
    then1.next = NULL;
    
    struct rule_type rule1;
    rule1.if_side = &if11;
    rule1.then_side = &then1;
    rule1.next = &rule2;
    
    Rule_Base = &rule1;
    
    
    
}


int fuzzy()
{
    
	
    fuzzification();
    rule_evaluation();
    defuzzification();
	return(outPutToESC(pwm.value));
    
}