#ifndef __SOP_Density_h__
#define __SOP_Density_h__
#include <SOP/SOP_Node.h>

namespace HDK_name {
class SOP_Density : public SOP_Node
{
	//public class functions and variable
	public:
		//Contructor and de-constructor
			SOP_Density(OP_Network *net, const char *name, OP_Operator *op);
		virtual ~SOP_Density();
		
		//Parameter list varaiable
		static PRM_Template myTemplateList[];
		
		//Internal Houdini Plugin Contructor, when nodes is getting created
		static OP_Node *myConstructor(OP_Network*, const char *, OP_Operator *);
	
	
	protected:
		//the actual cooking of the node
		virtual OP_ERROR cookMySop(OP_Context &context);
		
	
	
	private:
		fpreal RADIUS(fpreal t){
			//paramater evalFloat(Name of parm - not display name,if more then on entry, this is entry number,the time to evaluate it, usally its context.time())
			return evalFloat("rad",0,t);
		}
		int NEIGHBOURS(fpreal t){
			
			return evalInt("neighbours",0,t);
			
		}
		void ATTRIBNAME(UT_String &str, fpreal t){
			
			evalString(str,"attr",0,t);
			
		}
	
	
	
	
};
}

#endif