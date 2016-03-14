//First try of the new Density calc
//The SOP_Density SOP


#include <UT/UT_DSOVersion.h>
#include <SYS/SYS_Math.h>
#include <GU/GU_Detail.h>
#include <GEO/GEO_AttributeHandle.h>
#include <GEO/GEO_PointTree.h>
#include <PRM/PRM_Include.h>
#include <OP/OP_AutoLockInputs.h>
#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>
#include "SOP_Density.h"
#include "/usr/include/c++/4.2.1/backward/iostream.h"
#include "/usr/include/c++/4.2.1/backward/fstream.h"

using namespace HDK_name;
void newSopOperator(OP_OperatorTable *table){
	table->addOperator( 
					new OP_Operator(
							"pt_density", //internal node name c++
							"Point_Density", // Houdini Node Name in SOP:s
							SOP_Density::myConstructor, // How to build the SOP
							SOP_Density::myTemplateList, // Paramaters on the node
							1, // Min number of inputs
							1, // Max number of outputs
							0)
					);	
}

// define parameters names, values etc
// PRM_Default = A parameter's default value on creation
// PRM_Name = A parameters internal name and actual UI name

static PRM_Name radiusName("rad","Radius");
static PRM_Name neighName("neighbours","Neighbours");
static PRM_Name attrName("attr","Density Attribute Name");
static PRM_Range  radiusRange(PRM_RANGE_UI, 0.0001,PRM_RANGE_UI, 100.0);
static PRM_Range neighRange(PRM_RANGE_UI,1,PRM_RANGE_UI,1000);
static PRM_Default radiusDefault(0.1);
static PRM_Default attrDefault(0,"point_density");
static PRM_Default neighDefault(10);

PRM_Template SOP_Density::myTemplateList[] = {
	PRM_Template(PRM_FLT_J, 1 ,&radiusName ,&radiusDefault,0, &radiusRange),
	PRM_Template(PRM_INT_J, 1 ,&neighName ,&neighDefault, 0, &neighRange),
	PRM_Template(PRM_STRING,1 ,&attrName , &attrDefault,0,0),
	PRM_Template(), // MAKE SUR IT ALWAYS ENDS WITH A EMPTY TEMPLATE AND , "comma"
	};

//myContructor Implemetation, just return the node	
OP_Node *SOP_Density::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
	{
		return new SOP_Density(net,name,op);
	}

//Contructor implementation
SOP_Density::SOP_Density(OP_Network *net, const char *name, OP_Operator *op)
	: SOP_Node(net, name, op)
	{
		//std::cout<<"you made it"<<endl;
	}

//De-Construtor implemenation, usually nothing here
SOP_Density::~SOP_Density() {
	//std::cout<<"your done"<<endl;
}

OP_ERROR SOP_Density::cookMySop(OP_Context &context)

{
	//Lock inputs and cehck everything is ok
	OP_AutoLockInputs inputs(this);
	if (inputs.lock(context) >= UT_ERROR_ABORT){
		return error();
	}
	
	
	
	
	// duplicate source from input 0 
	duplicateSource(0,context);
	flags().timeDep = 1;
	
	//Varaibles
	fpreal ctime = context.getTime();//ctime is the currrent time e.g frame 1020 in a 24fps project is 1020.0/24 = 42.666666667
	float radius = SOP_Density::RADIUS(ctime);
	int neighbours = SOP_Density::NEIGHBOURS(ctime);
	UT_String attrstr;
	SOP_Density::ATTRIBNAME(attrstr,ctime);
	
	//create the actual attribute to store the density
	GA_RWHandleF densityAttr(gdp->addFloatTuple(GA_ATTRIB_POINT,attrstr,1));
	
	//create ppoint tree object
	GEO_PointTreeGAOffset pointtree;
	pointtree.build(gdp,NULL);
	
	GA_Offset ptoff;
	GA_FOR_ALL_PTOFF(gdp,ptoff){
		
		GEO_PointTree::IdxArrayType pointlist; //generate array to put points from ptree in
		GEO_PointTree::IdxArrayType distancelist; // generate array to put distance from ptree
		
		
		UT_Vector3 pos = gdp->getPos3(ptoff); //vector pos for points current position
		UT_FloatArray distance; //Array to store the calculated distance
		
		//ptree.findAllCloseIdx(pos,radius,pointlist) //find all points within the given radius
		int numclosept = pointtree.findNearestGroupIdx(pos,radius,neighbours,distancelist,distance);
		float weight = 1.0/numclosept;
		float sum = 0.0; //total sum of distance
		
		for(int i = 0;i<numclosept;++i){
			sum += distance(i);
		}
		sum *= weight;
		if(numclosept >= 1){
			densityAttr.set(ptoff,sum);
		}else{
			densityAttr.set(ptoff,-1.0);
		}
		
		
		
	}
		return error();
	}