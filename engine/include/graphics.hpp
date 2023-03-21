#ifndef _GRAPHICS_HPP_
#define _GRAPHICS_HPP_

	#include <geometry.hpp>
	#include <list>
	#include <string>
	
	namespace VoidEngine{
		
		class RGBColor{
		public:
			float r;
			float g;
			float b;
			RGBColor();
			RGBColor(float, float, float);
			RGBColor(const RGBColor & );
			RGBColor & operator= (const RGBColor & );
		};
		
		class RGBObject{
			private:
			RGBColor color;
				
			public:
			RGBObject();
			RGBObject(const RGBObject &);
			~RGBObject();
			void setColor(const RGBColor &);
			const RGBColor & getColor() const;
			
		};
		
		class GraphicObject : public Transformation2d, public VectorFormater2,
		{
			public:
			enum Type
			{
				T_GROUP,
				T_LINE,
				T_POLYLINE,
				T_SHAPE,
				T_RECT,
				T_CIRCLE,
				T_SPRITE,
				T_LINEGROUP,
				T_RECTGROUP
			};

			protected:
			Type type;
			
			public:
			bool visible;

			GraphicObject(Type );
			~GraphicObject();

			virtual GraphicObject * cloneGraphicObject() const = 0;
			virtual void freeGraphicObject() = 0;
				
			Type getType() const;
		};
	
		class BorderedGraphicObject
		{
			private:
			bool borderEnable;
			RGBColor borderColor;
		
			public:
			BorderedGraphicObject();
			BorderedGraphicObject(const BorderedGraphicObject &);
			~BorderedGraphicObject();

			bool isBorderEnable() const;
			const RGBColor & getBorderColor() const;
			void setBorder(bool );
			void setBorderColor(const RGBColor &);
		};
		
		class SolidGraphicObject
		{
			private:
			bool solidEnable;
			RGBColor fillColor;
			
			public:
			SolidGraphicObject();
			SolidGraphicObject(const SolidGraphicObject &);
			~SolidGraphicObject();
			
			bool isSolid() const;
			const RGBColor & getFillColor() const;	
			void setSolid(bool );
			void setFillColor(const RGBColor &);	
		};
	
		class GraphicGroup : public GraphicObject
        {
			public:
            typedef std::list<GraphicObject *>::iterator iterator; 
			typedef std::list<GraphicObject *>::const_iterator const_iterator;

            private:
			std::list<GraphicObject *> objects;

			public:
			GraphicGroup();
			GraphicGroup(const GraphicGroup &);
			~GraphicGroup();

			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;

			/*Truncated list interface*/
			size_t size() const;
			const_iterator begin() const;
			const_iterator end() const;
			void push_back(GraphicObject *);
			void clear();

		};
		
		class GraphicPolyline : public GraphicObject, 
			public VectorArray2, public RGBObject
		{
			public:
			GraphicPolyline();
			GraphicPolyline(const GraphicPolyline & );
			~GraphicPolyline();

			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;
		};
		
		class GraphicShape : public GraphicObject, public VectorArray2, 
			public BorderedGraphicObject, public SolidGraphicObject
		{
			public:
			GraphicShape();
			GraphicShape(const GraphicShape & );
			~GraphicShape();

			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;
		};
		
		class GraphicLine : public GraphicObject, public Line, 
			public RGBObject
		{
			public:
			GraphicLine();
			GraphicLine(const GraphicLine & );
			~GraphicLine();

			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;
		};
		
		
		class GraphicRectangle : public GraphicObject, public Rectangle,
			public BorderedGraphicObject, public SolidGraphicObject
		{
			public:
			GraphicRectangle();
			GraphicRectangle(const GraphicRectangle & );
			~GraphicRectangle();
			
			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;

		};
		
		class GraphicCircle : public GraphicObject, public Circle,
			public BorderedGraphicObject, public SolidGraphicObject
		{
			public:
			GraphicCircle();
			GraphicCircle(const GraphicCircle & );
			~GraphicCircle();

			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;
		};
		

		class GraphicSprite : public GraphicObject, public Rectangle
		{
			private:
			std::string textureStrId;
			Transformation2d textureWindowTransformation;
			Rectangle textureWindow;

			public:
			GraphicSprite();
			GraphicSprite(const GraphicSprite & );
			~GraphicSprite();

			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;

		
			const std::string & getTextureStrId();
			const Rectangle & getTextureWindow();
			const Transformation2d & getTextureWindowTransformation();

			void setTextureStrId(const std::string &);
			void setTextureWindow(const Rectangle &);
			void setTextureWindowTransformation(const Transformation2d &);
		};
		
		class GraphicLineGroup : public GraphicObject{
			private:
			std::list<GraphicLine *> lines;
			
			public:
			GraphicLineGroup();
			GraphicLineGroup(const GraphicLineGroup &);
			~GraphicLineGroup();

			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;

			/*Truncated list interface*/
			size_t size() const;
			const_iterator begin() const;
			const_iterator end() const;
			void push_back(GraphicLine *);
			void clear();
		};
		
		class GraphicRectangleGroup : public GraphicObject{
			private:
			std::list<GraphicRectangle *> rectangles;
			
			public:
			GraphicRectangleGroup();
			GraphicRectangleGroup(const GraphicRectangleGroup& );
			~GraphicRectangleGroup();
			
			/*GraphicObject interface*/
			GraphicObject * cloneGraphicObject() const;
			void freeGraphicObject();

			/*VectorFormater2 interface*/
			void format(VectorArray2 &) const;
			void format(Vector2d *, size_t) const;
			size_t getFormatSize() const;

			/*Truncated list interface*/
			size_t size() const;
			const_iterator begin() const;
			const_iterator end() const;
			void push_back(GraphicRectangle *);
			void clear();

		};
		
		class GraphicModel: public GraphicGroup{
			public:
			GraphicModel();
			GraphicModel(const GraphicModel& );
			~GraphicModel();
					
		};
		
	}
	
#endif
