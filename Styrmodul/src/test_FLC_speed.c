/*
 * test_FLC_speed.c
 *
 * Created: 4/12/2017 8:50:15 AM
 *  Author: hjaek237
 */ 

//
//  main.c
//  fis
//
//  Created by Mathias Dalshagen on 2017-04-08.
//  The code for the FIS was fetched from: http://www.drdobbs.com/cpp/fuzzy-logic-in-c/184408940
//

#include <stdio.h>
#include <string.h>




void FLC_road(void);
int min(arg1,arg2);
int maxi(arg1,arg2);
void compute_degree_of_membership(mf,input);
int compute_area_of_trapezoid(mf);
void fuzzification();
void rule_evaluation();
void defuzzification();


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
#define UPPER_LIMIT  100    /* max number assigned as degree of membership */

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



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


/* Returns the maximal value of two integer arguments */
int maxi(arg1,arg2)
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

/* Returns the minimal value of two integer arguments */
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
            *(tp->value) = maxi(strength,*(tp->value));
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



////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////









/* Initialization of fuzzy speed controller for road mode 
 * The input variables are a (1) measurement of distance to closes
 * stop line or obstacle ("distance") and (2) measurement of speed.
 * Output variable is change in pwm for ESC.
 * @param mode - if 1 then run fuzzy(), if 0 then initialize
*/
void FLC_road(void)
{
    
    /* Declaration of input and output variables */
    
    struct io_type distance;    // input 1
    struct io_type speed;       // input 2
    struct io_type pwm;         // output
    
    strcpy(speed.name, "speed");
    strcpy(distance.name, "distance");
    strcpy(pwm.name, "pwm");
    
    
    
    
    /* MFS FOR THE DISTANCE INPUT VARIABLE */
    
    
    
    struct mf_type threeM;
    strcpy(threeM.name, "threeM");
    threeM.value = 0;
    threeM.point1 = 150;
    threeM.point2 = 301;
    threeM.slope1 = 2;
    threeM.slope2 = 100;
    threeM.next = NULL;
    
    struct mf_type twoM;
    strcpy(twoM.name, "twoM");
    twoM.value = 0;
    twoM.point1 = 110;
    twoM.point2 = 222;
    twoM.slope1 = 2;
    twoM.slope2 = 2;
    twoM.next = &threeM;

    
    struct mf_type oneM;
    strcpy(oneM.name, "oneM");
    oneM.value = 0;
    oneM.point1 = 22;
    oneM.point2 = 150;
    oneM.slope1 = 2;
    oneM.slope2 = 2;
    oneM.next = &twoM;
    
    
    struct mf_type stopDist;
    strcpy(stopDist.name, "stopDist");
    stopDist.value = 0;
    stopDist.point1 = -1;
    stopDist.point2 = 62;
    stopDist.slope1 = 100;
    stopDist.slope2 = 2;
    stopDist.next = &oneM;
    
    
    distance.membership_functions = &stopDist;
    distance.next = NULL;
    
    
    /* MFS FOR THE SPEED INPUT VARIABLE */
    
    
    struct mf_type high;
    strcpy(high.name, "high");
    high.value = 0;
    high.point1 = 2850;
    high.point2 = 2931;
    high.slope1 = 2;
    high.slope2 = 100;
    high.next = NULL;
    
    struct mf_type medium;
    strcpy(medium.name, "medium");
    medium.value = 0;
    medium.point1 = 2780;
    medium.point2 = 2900;
    medium.slope1 = 2;
    medium.slope2 = 2;
    medium.next = &high;
    
    
    struct mf_type low;
    strcpy(low.name, "low");
    low.value = 0;
    low.point1 = 2749;
    low.point2= 2830;
    low.slope1 = 100;
    low.slope2 = 2;
    low.next = &medium;
    
    speed.membership_functions = &low;
    speed.next = &distance;
    System_Inputs = &speed;
    
    
    /* MFS FOR THE PWM OUTPUT VARIABLE
     *
     * From matlab
     * speed interval [2750 2930]
     *
     * [2765 2790 2810 2836] -slowSpeed
     * [2800 2836 2844 2880 -cruise
     * [2844 2880 2890 2926] - medium
     * [2890 2926 2934 2970] - high
     *
     */
    
    struct mf_type noSpeed;
    strcpy(noSpeed.name, "noSpeed");
    noSpeed.value = 0;
    noSpeed.point1 = 2749;
    noSpeed.point2 = 2790;
    noSpeed.slope1 = 100;
    noSpeed.slope2 = 4;
    noSpeed.next = NULL;
    
    struct mf_type slow;
    strcpy(slow.name, "slow");
    slow.value = 0;
    slow.point1 = 2765;
    slow.point2 = 2836;
    slow.slope1 = 4;
    slow.slope2 = 4;
    slow.next = &noSpeed;
    
    struct mf_type cruise;
    strcpy(cruise.name, "cruise");
    cruise.value = 0;
    cruise.point1 = 2800;
    cruise.point2 = 2880;
    cruise.slope1 = 3;
    cruise.slope2 = 3;
    cruise.next = &slow;
    
    struct mf_type medHigh;
    strcpy(medHigh.name, "medHigh");
    medHigh.value = 0;
    medHigh.point1 = 2844;
    medHigh.point2 = 2926;
    medHigh.slope1 = 3;
    medHigh.slope2 = 3;
    medHigh.next = &cruise;
      
    struct mf_type max;
    strcpy(high.name, "high");
    max.value = 0;
    max.point1 = 2890;
    max.point2 = 2931;
    max.slope1 = 4;
    max.slope2 = 100;
    max.next = &medHigh;
    
    pwm.membership_functions = &max;
    System_Outputs = &pwm;
    pwm.next = NULL;
    
    /* The rule-base
     *
     * From Matlab
     * 
     * #1 if distance is "stoppingDistance" then speed is "noSpeed"
     * #2 if speed is "slow" and distance is "oneM" then speed is "slow"
     * #3 if speed is "medium" and distance is "oneM" then speed is "cruising"
     * #4 if speed is "high" and distance is "oneM" then speed is "cruising"
     * #5 if distance is "threeM" then speed is "max"
     * #6 if distance is "twoM" then speed is "medHigh"
     *
     *
     */
    
    
    /* rule # 6: if twoM then medHigh */
    struct rule_element_type then6;
    then6.value = &medHigh.value;
    then6.next = NULL;
    
    struct rule_element_type if61;
    if61.value = &twoM.value;
    
    struct rule_type rule6;
    rule6.if_side = &if61;
    rule6.then_side = &then6;
    rule6.next = NULL;
    
    /* rule # 5: if threeM then max */
    
    struct rule_element_type then5;
    then5.value = &max.value;
    then5.next = NULL;
    
    
    struct rule_element_type if51;
    if51.value = &threeM.value;
    if51.next = NULL;
    
    struct rule_type rule5;
    rule5.if_side = &if51;
    rule5.then_side = &then5;
    rule5.next = &rule6;
    
    /* rule # 4: if high and oneM then cruise */
    
    struct rule_element_type then4;
    then4.value = &cruise.value;
    then4.next = NULL;
    
    struct rule_element_type if42;
    if42.value = &oneM.value;
    if42.next = NULL;
    
    struct rule_element_type if41;
    if41.value = &medium.value;
    if41.next = &if42;
    
    struct rule_type rule4;
    rule4.if_side = &if41;
    rule4.then_side = &then4;
    rule4.next = &rule5;
    
    
    /* rule # 3: if medium and oneM then cruise */
    
    struct rule_element_type then3;
    then3.value = &cruise.value;
    then3.next = NULL;
    
    struct rule_element_type if32;
    if32.value = &oneM.value;
    if32.next = NULL;
    
    struct rule_element_type if31;
    if31.value = &medium.value;
    if31.next = &if32;
    
    struct rule_type rule3;
    rule3.if_side = &if31;
    rule3.then_side = &then3;
    rule3.next = &rule4;
    
    
    /* rule # 2: if low and oneM then slow */
    
    struct rule_element_type if22;
    if22.value = &oneM.value;
    if22.next = NULL;
    
    struct rule_element_type if21;
    if21.value = &low.value;
    if21.next = &if22;
    
    struct rule_element_type then2;
    then2.value = &slow.value;
    then2.next = NULL;
    
    struct rule_type rule2;
    rule2.if_side = &if21;
    rule2.then_side = &then2;
    rule2.next = &rule3;
    
    /* rule # 1: if stopDist then noSpeed */
    
    struct rule_element_type if11;
    if11.value = &stopDist.value;
    if11.next = NULL;
    
    struct rule_element_type then1;
    then1.value = &noSpeed.value;
    then1.next = NULL;
    
    struct rule_type rule1;
    rule1.if_side = &if11;
    rule1.then_side = &then1;
    rule1.next = &rule2;
    
    Rule_Base = &rule1; // pointer to rule1
    
    //    initialize_fis();
    
    speed.value = 2900;
    distance.value = 280;
    fuzzification();
    rule_evaluation();
    defuzzification();
    
    printf("%d\n", pwm.value);
    
}





