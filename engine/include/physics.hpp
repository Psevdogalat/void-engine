#ifndef _PHYSICS_HPP_
#define _PHYSICS_HPP_
	
	#include <geometry.hpp>
	
	class PhysicalModel
	{
		public:
		bool		dinamic;
		
		Vector2d 	position;
		Vector2d	normal;		

		Vector2d 	velocity;
		Vector2d	acceleration;
		Vector2d	force;
		Vector2d	pulse;
		
		double 		angular_velocity;
		double 		torque;
		double 		mass;
		
		PhysicalModel();
		~PhysicalModel();
		
		void compute(double );
	};
	
#endif
