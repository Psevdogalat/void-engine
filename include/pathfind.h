#ifndef _PATHFIND_H_
#define _PATHFIND_H_

#include <engine.h>

#include <list>
#include <vector>
#include <cmath>
#include <algorithm>

namespace ENGINE{

class PATHFINDER{	
	public:
		class NODE;
		
		/* --EDGE */
		struct EDGE{
			NODE* 		neighbour 	= nullptr;
			double		distance 	= 0;
		};
		/* EDGE-- */
		
		/* NODE-- */
		class NODE{
			public:			
				enum STATE{
					ST_UNDEFINE,
					ST_BARRED,
					ST_UNBARRED
				};
				
				STATE				state;
				NODE*				parent;
				VECTOR2D			position;
				double				weight;
				std::list<EDGE>		links;
				
				NODE();
				NODE(const NODE& Smpl);
				~NODE();
				
				bool
				is_barred
				() const;
				
				bool 
				is_linked
				(
					const NODE&
				)const;
				
				void 
				link
				(
					const NODE&
				);
		};
		/* --NODE */
		
		/* GRID-- */
		class GRID{
			public:
			
			/* INDEX -- */
			struct INDEX{
				
				UINT x;
				UINT y;
				
				INDEX();
				INDEX(UINT X, UINT Y);
				INDEX(const INDEX& Smpl);
				
				bool  	operator== 	(const INDEX& Rval )const;
				bool  	operator!= 	(const INDEX& Rval )const;
				INDEX& 	operator=	(const INDEX& Rval );	
			};
			/* --INDEX */
			
			GRID();
			GRID(UINT Width, UINT Height);
			GRID(const GRID& Smpl);
			~GRID();
			
			bool init (UINT width, UINT height );
			UINT get_width () const;
			UINT get_height() const;
			bool empty() const;
			bool is_valid(const INDEX Index ) const;
			NODE* get_node(const INDEX Index ) const;
			std::vector<INDEX> get_neighbours(const INDEX Index) const;
			NODE* new_node(const INDEX Index );
			void clean ();
			
			GRID& operator= ( const GRID& Rval );
			
			protected:
				
			NODE**	ptrs;
			NODE*	base;
			NODE*	bud;
			UINT	width;
			UINT	height;
			UINT	length;
			
			void  free_storage();
			UINT  line_index(INDEX Index ) const;
			void* relocate(void* Old_base, void* Ptr ) const;
			
		};
		/* --GRID */
		
		
		enum EVALUATOR{
			EV_DIJKSTRA = 0,
			EV_EVRISTIC = 1,
			EV_ASTAR	= 2
		};
		
		struct CONFIGURATION{
			EVALUATOR			evaluator;
			UINT				width;
			UINT 				height;
			const SCENE*		scene;
			GAME_OBJECT_FILTER 	filter;
			COLLISION_MODEL		model;
			TRNSF_OBJECT		transform;
		};
	
		PATHFINDER();
		PATHFINDER(const PATHFINDER& );
		PATHFINDER(const CONFIGURATION& );
		~PATHFINDER();
		
		bool 
		configure
		(
			const CONFIGURATION& Config
		);
		
		bool 
		is_configured
		() const;
		
		const GRID&
		get_grid
		() const;

        bool 
        set_transform
        (
         const TRNSF_OBJECT& 
        );

        bool
        set_evaluator
        (
         const EVALUATOR 
        );
		
		bool 
		test_point
		(
			const VECTOR2D&		Point,
			GRID::INDEX*		Index
		) const;
		
		std::vector<VECTOR2D>
		get_path
		(
			const VECTOR2D& StartPoint,
			const VECTOR2D& EndPoint
		);
		
	protected:
		typedef bool (*PATH_EVAL_FNC)
		(
			const NODE&, 
			const NODE&, 
			const VECTOR2D& 
		);
		
		
		bool			cfg_ok;
		CONFIGURATION	cfg;
		GRID			grid;
		VECTOR2D		offset;
		
		static bool 
		evl_dijkstra
		(
			const NODE& 	Node1,
			const NODE& 	Node2,
			const VECTOR2D&	End_position
		);

		static bool 
		evl_evristic
		(
			const NODE& 	Node1,
			const NODE& 	Node2,
			const VECTOR2D&	End_position
		);

		static bool 
		evl_astar
		(
			const NODE& 	Node1,
			const NODE& 	Node2,
			const VECTOR2D&	End_position
		);
		
		std::list<NODE*>
		find_path
		(
			const GRID::INDEX& 		Start_index,
			const GRID::INDEX&		End_index,
			PATH_EVAL_FNC			Evaluator
		);
		
		bool 
		scan_node
		(
			NODE& Node 
		);
		
		VECTOR2D
		calc_position
		(
			GRID::INDEX Index
		) const;
		
};

}

#endif
