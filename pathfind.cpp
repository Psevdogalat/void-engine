#include <pathfind.h>
#include <limits.h>
#include <stack>
#include <stdio.h>

using namespace ENGINE;

PATHFINDER::GRID::INDEX::INDEX
()
{
	x = 0;
	y = 0;
}

PATHFINDER::GRID::INDEX::INDEX
(
	const PATHFINDER::GRID::INDEX& Smpl
)
{
	x = Smpl.x;
	y = Smpl.y;
}

PATHFINDER::GRID::INDEX::INDEX
(
	UINT X, UINT Y
)
{
	x = X;
	y = Y;
}

bool
PATHFINDER::GRID::INDEX::operator==
(
	const PATHFINDER::GRID::INDEX& Rval
) const
{
	if(x == Rval.x && y == Rval.y)
		return true;
	
	return false;
}

bool
PATHFINDER::GRID::INDEX::operator!=
(
	const PATHFINDER::GRID::INDEX& Rval
) const
{
	return !(*this == Rval);
}

PATHFINDER::GRID::INDEX&
PATHFINDER::GRID::INDEX::operator=
(
	const PATHFINDER::GRID::INDEX& Rval
)
{
	x = Rval.x;
	y = Rval.y;
	return *this;
}


//------------------------------------------------------------------------------
PATHFINDER::GRID::GRID
()
{
	ptrs 	= nullptr;
	base	= nullptr;
	bud		= nullptr;
	width 	= 0;
	height	= 0;
	length	= 0;
}

PATHFINDER::GRID::GRID
(
	const PATHFINDER::GRID& Smpl
):GRID(Smpl.width, Smpl.height)
{
	bud = (NODE*) relocate(Smpl.base, Smpl.bud);
	
	for(UINT i = 0; i < length; i++){
		if(Smpl.ptrs[i] == nullptr)continue;
		ptrs[i] = (NODE*) relocate(Smpl.base, Smpl.ptrs[i]);
	}
	
	NODE*	src_ptr;
	NODE*	dst_ptr;
	
	src_ptr = Smpl.bud;
	dst_ptr = bud;
	
	while(dst_ptr != base){
		src_ptr--;
		dst_ptr--;
		
		new(dst_ptr) NODE(*src_ptr);
		
		if(dst_ptr->parent)
			dst_ptr->parent = (NODE*) relocate(Smpl.base, dst_ptr->parent);

		for(
			std::list<EDGE>::iterator link = dst_ptr->links.begin();
			link != dst_ptr->links.end(); link++
		)
			link->neighbour =  (NODE*) relocate(Smpl.base, link->neighbour);
				
	}
	
}

void* 
PATHFINDER::GRID::relocate
(
	void* Old_base,
	void* Ptr
) const
{
	return ((unsigned char*)base) + ((unsigned char*)Ptr - (unsigned char*)Old_base);
}

PATHFINDER::GRID::GRID
(
	const UINT Width,
	const UINT Height
):GRID()
{
	
	UINT i;
	
	if(Width != 0 && Height != 0 && UINT_MAX/Width > Height){
		
		width 	= Width;
		height	= Height;
		length 	= width * height;
		
		ptrs 	= new NODE* [length];
		base 	= (NODE*)malloc(sizeof(NODE) * length);
		bud 	= base;
		
		for(i = 0; i < length; i++)
			ptrs[i] = nullptr;
	}
}

PATHFINDER::GRID::~GRID
()
{
	if(!length)
		return;
	
	free_storage();
	
	delete [] ptrs;
	free(base);
}

bool
PATHFINDER::GRID::init
(
	UINT Width,
	UINT Height
)
{	
	if(Width == width && Height == height){
		clean();
		
	}else{
		this->~GRID();
		new(this) GRID(Width, Height);
		
	}
	
	return true;
}

UINT 
PATHFINDER::GRID::get_width
() const
{
	return width;
}

UINT 
PATHFINDER::GRID::get_height
() const
{
	return height;
}

PATHFINDER::NODE*
PATHFINDER::GRID::get_node
(
	const PATHFINDER::GRID::INDEX Index
) const
{
	if(!length)
		return nullptr;
	
	if(!is_valid(Index))
		return nullptr;
	
	return ptrs[line_index(Index)];
}


PATHFINDER::NODE*
PATHFINDER::GRID::new_node
(
	const PATHFINDER::GRID::INDEX Index
)
{
	UINT  li;
	
	if(!length)
		return nullptr;
	
	if(!is_valid(Index))
		return nullptr;
	
	li = line_index(Index);
	if(ptrs[li] != nullptr)
		return nullptr;
	
	ptrs[li] = new(bud) NODE();
	bud ++;
	
	return ptrs[li];
}

void 
PATHFINDER::GRID::clean
()
{
	for(UINT i = 0; i < length; i++)
		ptrs[i] = nullptr;
	
	free_storage();	
}

void 
PATHFINDER::GRID::free_storage
()
{	
	while(bud != base)
		(--bud)->~NODE();
	
}

bool  
PATHFINDER::GRID::empty
()const
{
	return bud == base;
}


bool
PATHFINDER::GRID::is_valid
(
	const PATHFINDER::GRID::INDEX Index
) const
{
	return (Index.x >= width || Index.y >= height)?false:true;
}

UINT 
PATHFINDER::GRID::line_index
(
	const PATHFINDER::GRID::INDEX Index
) const
{
	return Index.y * width + Index.x;
}

PATHFINDER::GRID&
PATHFINDER::GRID::operator=
(
	const PATHFINDER::GRID& Rval
)
{
	this->~GRID();
	new(this) GRID(Rval);
	
	return *this;
}

std::vector<PATHFINDER::GRID::INDEX> 
PATHFINDER::GRID::get_neighbours
(
	const PATHFINDER::GRID::INDEX Index
)const
{
	static const UINT 	nghbrs_n 	= 8;	
	static const struct{
		UINT	r1;
		UINT	r2;
	}rule_map[nghbrs_n] = 
	{
		{0,3},
		{3,3},
		{1,3},
		{0,0},
		{1,1},
		{0,2},
		{2,2},
		{1,2}
	};
	
	bool 	disables[nghbrs_n];
	bool	rules[4];
	UINT 	i,j,n;
	INDEX	neighbour;
	std::vector<INDEX> rvector;
	
	if(is_valid(Index)){
	
		n = 0;
		j = 0;
		rules[0] = Index.x == 0;
		rules[1] = Index.x == width -1;
		rules[2] = Index.y == 0;
		rules[3] = Index.y == height -1;
		
		for(i = 0; i < nghbrs_n; i++)
			if(!(disables[i] = rules[rule_map[i].r1] || rules[rule_map[i].r2]))
				n++;
		
		rvector = std::vector<INDEX>(n);
			
		for(i = 0; i < nghbrs_n; i++){
			if(disables[i])
				continue;
				
			switch(i){
				case 0: neighbour = INDEX(Index.x -1,Index.y +1); break;
				case 1: neighbour = INDEX(Index.x   ,Index.y +1); break;
				case 2: neighbour = INDEX(Index.x +1,Index.y +1); break;
				case 3: neighbour = INDEX(Index.x -1,Index.y   ); break;
				case 4: neighbour = INDEX(Index.x +1,Index.y   ); break;
				case 5: neighbour = INDEX(Index.x -1,Index.y -1); break;
				case 6: neighbour = INDEX(Index.x   ,Index.y -1); break;
				case 7: neighbour = INDEX(Index.x +1,Index.y -1); break;
			}
			
			rvector[j++] = neighbour;
		}
	}
	
	return rvector;
}

//------------------------------------------------------------------------------

PATHFINDER::NODE::NODE
()
{
	state 		= ST_UNDEFINE;
	weight 		= 0;
	parent 		= nullptr;
	position	= vector2d(0.0, 0.0);
}

PATHFINDER::NODE::NODE
(
	const PATHFINDER::NODE& Smpl
)
{
	state 		= Smpl.state;
	weight		= Smpl.weight;
	parent		= Smpl.parent;
	links		= Smpl.links;
	position	= Smpl.position;
	
}

PATHFINDER::NODE::~NODE
()
{
	
}

bool
PATHFINDER::NODE::is_barred
() const
{
	return state == ST_BARRED;
}

bool 
PATHFINDER::NODE::is_linked
(
	const NODE& Node
)const
{
	for(const EDGE& link: links)
		if(link.neighbour == &Node)
			return true;
	
	return false;
}

void 
PATHFINDER::NODE::link
(
	const NODE&	Node
)
{
	EDGE new_link;
	new_link.distance 	= vector_length(position - Node.position);
	new_link.neighbour 	= (NODE*)&Node;
	
	links.push_back(new_link);
}

//------------------------------------------------------------------------------
PATHFINDER::PATHFINDER
()
{
	cfg_ok 	= false;
}

PATHFINDER::PATHFINDER
(
	const PATHFINDER& Smpl
)
{
	cfg_ok 	= Smpl.cfg_ok;
	cfg		= Smpl.cfg;
	grid	= Smpl.grid;
	
}

PATHFINDER::PATHFINDER
(
	const CONFIGURATION& Cfg
):PATHFINDER()
{
	configure(Cfg);
}

PATHFINDER::~PATHFINDER
()
{
	
}
	
bool 
PATHFINDER::configure
(
	const CONFIGURATION& Config
)
{
	if(
		Config.width 	== 0 || 
		Config.height 	== 0
	)
		return false;
	
	if(
		Config.transform.get_scale().x == 0.0 || 
		Config.transform.get_scale().y == 0.0
	)
		return false;
		
	if(Config.scene == nullptr)
		return false;
	
	cfg = Config;
	
	cfg_ok = false;
	if(!grid.init(cfg.width, cfg.height))
		return false;
	
	offset = vector2d(0.5, 0.5) - 0.5* vector2d(
			(double)grid.get_width(), 
			(double)grid.get_height()
		);
	
	cfg_ok = true;
	return true;
}

bool 
PATHFINDER::is_configured
() const
{
	return cfg_ok;
}

const PATHFINDER::GRID&
PATHFINDER::get_grid
() const
{
	return grid;
}

bool 
PATHFINDER::test_point
(
	const VECTOR2D&		Point,
	GRID::INDEX*		Index
) const
{
	if(!cfg_ok)
		return false;
	
    MATRIX33 tmat;
	VECTOR2D inner_coord;
	VECTOR2D size_v;
	
	tmat = mat33_scl(
			vector2d(
				(double)grid.get_width(), 
				(double)grid.get_height()
			)
		);; 
	
	inner_coord = cfg.transform.get_backward_matrix() * Point;
	
	if(
		inner_coord.x < -0.5 ||
		inner_coord.x >  0.5 ||		
		inner_coord.y < -0.5 ||
		inner_coord.y >  0.5		
	)
		return false;
	
	size_v = vector2d((double)grid.get_width(), (double)grid.get_height());
	
	inner_coord = size_v * inner_coord - 
		offset + vector2d(0.5, 0.5);
	
	
	if(Index != nullptr){
		Index->x = (UINT)floor(inner_coord.x);
		Index->y = (UINT)floor(inner_coord.y);
	}
	
	return true;
}

bool
PATHFINDER::set_transform
(
 const TRNSF_OBJECT& Transform
)
{
    if(
            !cfg_ok || 
            Transform.get_scale().x == 0 ||
            Transform.get_scale().y == 0
      )
        return false;

    cfg.transform = Transform;
    return true;
}

bool 
PATHFINDER::set_evaluator
(
    const PATHFINDER::EVALUATOR Evaluator

) 
{
    if(!cfg_ok)
        return false;

    cfg.evaluator = Evaluator;
    return true;    
}
        
std::vector<VECTOR2D>
PATHFINDER::get_path
(
	const VECTOR2D& StartPoint,
	const VECTOR2D& EndPoint
)
{
	std::vector<VECTOR2D> 	rpath;
	UINT					path_len;
	std::list<NODE*>		node_chain;
	GRID::INDEX				start_index;
	GRID::INDEX				end_index;
	MATRIX33				tmat;
	PATH_EVAL_FNC			eval_fcn;
	
	if(
		cfg_ok &&
		test_point(StartPoint, &start_index) && 
		test_point(EndPoint, &end_index)
	){
		
		tmat = cfg.transform.get_forward_matrix() * mat33_scl(
				vector2d(
					1.0 / (double)grid.get_width(), 
					1.0 / (double)grid.get_height()
				)
			);
		
		switch(cfg.evaluator){
			case EV_DIJKSTRA: 	eval_fcn = evl_dijkstra; 	break;
			case EV_EVRISTIC: 	eval_fcn = evl_evristic; 	break;
			case EV_ASTAR: 		eval_fcn = evl_astar; 		break;
			default: 			eval_fcn = evl_dijkstra;
		}
		
		node_chain = find_path(start_index, end_index, eval_fcn);
		

		if(node_chain.size() > 0){
			path_len = node_chain.size() + 2;
			rpath = std::vector<VECTOR2D>(path_len);
			rpath[--path_len] = EndPoint;
			
			for(
				std::list<NODE*>::iterator it = --node_chain.end();
				true; it--
			){
				rpath[--path_len] = tmat * (*it)->position;
				if(it == node_chain.begin())
					break;
			}
			
			rpath[--path_len] = StartPoint;
		}
	}
	
	return rpath;
}


bool 
PATHFINDER::evl_dijkstra
(
	const PATHFINDER::NODE& Node1,
	const PATHFINDER::NODE& Node2,
	const VECTOR2D&			End_position
)
{
	
	return Node1.weight < Node2.weight;;
}

bool 
PATHFINDER::evl_evristic
(
	const PATHFINDER::NODE& Node1,
	const PATHFINDER::NODE& Node2,
	const VECTOR2D&			End_position
)
{
	
	return 
		vector_length(End_position - Node1.position) < 
		vector_length(End_position - Node2.position);
}

bool 
PATHFINDER::evl_astar
(
	const PATHFINDER::NODE& Node1,
	const PATHFINDER::NODE& Node2,
	const VECTOR2D&			End_position
)
{
	
	return 
		(vector_length(End_position - Node1.position) + Node1.weight) < 
		(vector_length(End_position - Node2.position) + Node2.weight);
}

std::list<PATHFINDER::NODE*>
PATHFINDER::find_path
(
	const PATHFINDER::GRID::INDEX& 	Start_index,
	const PATHFINDER::GRID::INDEX&	End_index,
	PATH_EVAL_FNC					Evaluator
)
{
	std::list<NODE*> 					rlist;
	std::list<GRID::INDEX> 				check;	
	GRID::INDEX							cur_index;
	NODE*								start_node;
	NODE*								end_node;
	NODE* 								cur_node;
	NODE*								evl_node;
	NODE*								ngh_node;
	std::list<GRID::INDEX>::iterator 	it;
	std::list<GRID::INDEX>::iterator 	it_cur;
	VECTOR2D							end_position;
	
	if(!cfg_ok)
		return rlist;
	
	grid.clean();
	
	start_node 		        = grid.new_node(Start_index);
	start_node->position    = calc_position(Start_index);
	start_node->state       = scan_node(*start_node)?
						        NODE::ST_BARRED:
						        NODE::ST_UNBARRED;

	end_node 		= nullptr;
	end_position	= calc_position(End_index);

	check.push_back(Start_index);
	
	while(!check.empty()){
		
		it_cur = check.begin();
		for(it = ++check.begin(); it != check.end(); it++){
			cur_node = grid.get_node(*it_cur);
			evl_node = grid.get_node(*it);
			
			if(Evaluator(*evl_node, *cur_node, end_position))
				it_cur = it;
		
		}
		
		cur_index 	= *it_cur;
		cur_node 	= grid.get_node(cur_index);
		check.erase(it_cur);
		
		if(cur_node->is_barred())
			continue;

		if(cur_index == End_index){
			end_node 	= cur_node;
			break;
		}
			
		for(GRID::INDEX ngh_index: grid.get_neighbours(cur_index)){
			
			ngh_node = grid.get_node(ngh_index);
			if(ngh_node == nullptr){
				ngh_node = grid.new_node(ngh_index);
				ngh_node->position 	= calc_position(ngh_index);				
				ngh_node->state = 
					(scan_node(*ngh_node))?
						NODE::ST_BARRED:
						NODE::ST_UNBARRED;
						
				ngh_node->parent = cur_node;
				ngh_node->weight = cur_node->weight + vector_length(
					ngh_node->position - cur_node->position
				);
				
				check.push_back(ngh_index);
			}
			
			if(!cur_node->is_linked(*ngh_node)){
				ngh_node->link(*cur_node);
				cur_node->link(*ngh_node);
			}
			
		}
		
	}

	cur_node = end_node;
	while(cur_node != nullptr){
		rlist.push_front(cur_node);
		cur_node = cur_node->parent;
	}	
	
	return rlist;
}

bool 
PATHFINDER::scan_node
(
	PATHFINDER::NODE& Node 
)
{
	MATRIX33 tmat;
	
	if(!cfg_ok)
		return false;
	
	tmat = cfg.transform.get_forward_matrix() * mat33_scl(
				vector2d(
					1.0 / (double)grid.get_width(), 
					1.0 / (double)grid.get_height()
				)
			);

	cfg.model.set_position(tmat * Node.position);
	cfg.model.set_normal(cfg.transform.get_normal());
	
	return cfg.scene->collision_test(cfg.model, &cfg.filter, nullptr);
}

VECTOR2D
PATHFINDER::calc_position
(
	PATHFINDER::GRID::INDEX Index
) const
{
	return vector2d((double)Index.x,(double)Index.y) + offset;
}
