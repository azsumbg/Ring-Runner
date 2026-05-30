#pragma once

#ifdef RINGHELP_EXPORTS
#define RINGHELP_API __declspec(dllexport)
#else
#define RINGHELP_API __declspec(dllimport)
#endif

#include <d2d1.h>
#include <random>

#pragma comment(lib, "d2d1.lib")

constexpr float scr_width{ 1000.0f };
constexpr float scr_height{ 800.0f };

constexpr float sky{ 50.0f };
constexpr float ground{ 700.0f };

constexpr float max_jump{ 100.0f };

constexpr int EX_PTR{ 6001 };
constexpr int EX_INDEX{ 6002 };
constexpr int EX_PARAM{ 6003 };
constexpr int EX_INIT{ 6004 };
constexpr int EX_UNK{ 6005 };

enum class dirs { left = 0, right = 1, stop = 3 };
enum class actions { run = 0, climb_up = 1, climb_down = 2, stop = 3, fall = 4, jump = 5 };
enum class creatures { zombie_girl = 0, zombie_boy = 1, zombie_flyer = 2, hero = 3 };
enum class fields { flat_ground = 0, left_slope = 1, right_slope = 2, background = 3, intro = 4, portal = 5 };
enum class assets { rings = 0, armor = 1, potion = 2 };

struct RINGHELP_API FADING
{
	assets type { assets::rings };

	D2D1_RECT_F view_rect{};

	int delay = 20;
	
	float opacity = 1.0f;

	float get_opacity()
	{
		--delay;
		if (delay <= 0)
		{
			delay = 20;
			opacity -= 0.1f;
		}
		return opacity;
	}
};

namespace dll
{
	class RINGHELP_API EXCEPTION
	{
	private:
		int _what = 0;

	public:

		EXCEPTION(int what);

		const wchar_t* eWhat()const;
	};

	class RINGHELP_API RANDIT
	{
	private:
		std::mt19937* twister{ nullptr };

	public:
		RANDIT();

		~RANDIT();

		int operator()(int min, int max);
		float operator()(float min, float max);
	};

	template<typename T> class BAG
	{
	private:
		size_t max_size{ 1 };
		T* basePtr{ nullptr };
		size_t next_pos{ 0 };

	public:
		BAG()
		{
			basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
			if (basePtr == nullptr)throw EXCEPTION(EX_INIT);
		};
		BAG(size_t lenght)
		{
			max_size = lenght;
			basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

			if (basePtr == nullptr)throw EXCEPTION(EX_INIT);
		}
		BAG(const BAG& other)
		{
			if (other.basePtr)
			{
				free(basePtr);
				basePtr = nullptr;

				max_size = other.max_size;
				next_pos = other.next_pos;

				basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
				
				if (basePtr == nullptr)throw EXCEPTION(EX_INIT);
				else if (next_pos > 0)
						for (size_t count = 0; count < next_pos; ++count)basePtr[count] = other.basePtr[count];
			}
			else
			{
				free(basePtr);
				max_size = 1;
				next_pos = 0;

				basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (basePtr == nullptr)throw EXCEPTION(EX_INIT);
			}
		}
		BAG(BAG&& other)
		{
			free(basePtr);
			basePtr = nullptr;

			if (other.basePtr)
			{
				max_size = other.max_size;
				next_pos = other.next_pos;
				basePtr = other.basePtr;

				if (next_pos > 0)
					for (size_t count = 0; count < next_pos; ++count)basePtr[count] = other.basePtr[count];
			}
			else
			{
				free(basePtr);
				max_size = 1;
				next_pos = 0;

				basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

				if (basePtr == nullptr)throw EXCEPTION(EX_INIT);
			}

			other.basePtr = nullptr;
		}

		~BAG()
		{
			free(basePtr);
		}

		BAG& operator=(const BAG& other)
		{
			if (other.basePtr)
			{
				free(basePtr);
				max_size = other.max_size;
				next_pos = other.next_pos;

				basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
				if (basePtr == nullptr)throw EXCEPTION(EX_INIT);
				else if (other.next_pos > 0)
					for (size_t count = 0; count < next_pos; ++count)basePtr[count] = other.basePtr[count];
			}
			else
			{
				free(basePtr);
				max_size = 1;
				next_pos = 0;

				basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));
				if (basePtr == nullptr)throw EXCEPTION(EX_INIT);
			}

			return *this;
		}
		BAG& operator=(BAG&& other)
		{
			if (other.basePtr != nullptr)
			{
				free(basePtr);
				max_size = other.max_size;
				next_pos = other.next_pos;

				basePtr = other.basePtr;
				other.basePtr = nullptr;
			}

			return *this;
		}

		T& operator[](size_t index)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (index <= 0)return *basePtr;
				else if (index >= next_pos)return basePtr[next_pos - 1];
			}

			return basePtr[index];
		}

		void clear()
		{
			free(basePtr);
			max_size = 1;
			next_pos = 0;

			basePtr = reinterpret_cast<T*>(calloc(max_size, sizeof(T)));

			if (!basePtr)throw EXCEPTION(EX_INIT);
		}
		bool empty() const
		{
			return(next_pos == 0);
		}

		size_t size() const
		{
			return next_pos;
		}
		size_t capacity()const
		{
			return max_size;
		}

		void push_front(T element)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (next_pos == 0)
				{
					*basePtr = element;
					++next_pos;
					return;
				}
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t cur_pos = next_pos; cur_pos > 0; --cur_pos)basePtr[cur_pos] = basePtr[cur_pos - 1];
						*basePtr = element;
						++next_pos;
						return;
					}
					else
					{
						++max_size;
						T* tempPtr = reinterpret_cast<T*>(realloc(basePtr, max_size * sizeof(T)));

						if (!tempPtr)throw EXCEPTION(EX_INIT);
						else
						{
							basePtr = tempPtr;
							for (size_t cur_pos = next_pos; cur_pos > 0; --cur_pos)basePtr[cur_pos] = basePtr[cur_pos - 1];
							*basePtr = element;
							++next_pos;
							return;
						}
					}
				}
			}
		}
		void push_front(T* element)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (next_pos == 0)
				{
					*basePtr = *element;
					++next_pos;
					return;
				}
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t cur_pos = next_pos; cur_pos > 0; --cur_pos)basePtr[cur_pos] = basePtr[cur_pos - 1];
						*basePtr = *element;
						++next_pos;
						return;
					}
					else
					{
						++max_size;
						T* tempPtr = reinterpret_cast<T*>(realloc(basePtr, max_size * sizeof(T)));

						if (!tempPtr)throw EXCEPTION(EX_INIT);
						else
						{
							basePtr = tempPtr;
							for (size_t cur_pos = next_pos; cur_pos > 0; --cur_pos)basePtr[cur_pos] = basePtr[cur_pos - 1];
							*basePtr = *element;
							++next_pos;
							return;
						}
					}
				}
			}
		}

		void push_back(T element)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					basePtr[next_pos] = element;
					++next_pos;
					return;
				}
				else
				{
					++max_size;

					T* tempPtr = reinterpret_cast<T*>(realloc(basePtr, max_size * sizeof(T)));

					if (!tempPtr)throw EXCEPTION(EX_INIT);
					else
					{
						basePtr = tempPtr;
						basePtr[next_pos] = element;
						++next_pos;
						return;
					}
				}
			}
		}
		void push_back(T* element)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (next_pos + 1 <= max_size)
				{
					basePtr[next_pos] = *element;
					++next_pos;
					return;
				}
				else
				{
					++max_size;

					T* tempPtr = reinterpret_cast<T*>(realloc(basePtr, max_size * sizeof(T)));

					if (!tempPtr)throw EXCEPTION(EX_INIT);
					else
					{
						basePtr = tempPtr;
						basePtr[next_pos] = *element;
						++next_pos;
						return;
					}
				}
			}
		}

		void insert(size_t index, T element)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (index < 0 || index > next_pos)return;
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t count = next_pos; count > index; --count)basePtr[count] = basePtr[count - 1];
						basePtr[index] = element;
						++next_pos;
						return;
					}
					else
					{
						++max_size;

						T* tempPtr = reinterpret_cast<T*>(realloc(basePtr, max_size * sizeof(T)));
						if (!tempPtr)throw EXCEPTION(EX_INIT);
						else
						{
							basePtr = tempPtr;
							for (size_t count = next_pos; count > index; --count)basePtr[count] = basePtr[count - 1];
							basePtr[index] = element;
							++next_pos;
							return;
						}
					}
				}
			}
		}
		void insert(size_t index, T* element)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (index < 0 || index > next_pos)return;
				else
				{
					if (next_pos + 1 <= max_size)
					{
						for (size_t count = next_pos; count > index; --count)basePtr[count] = basePtr[count - 1];
						basePtr[index] = *element;
						++next_pos;
						return;
					}
					else
					{
						++max_size;

						T* tempPtr = reinterpret_cast<T*>(realloc(basePtr, max_size * sizeof(T)));
						if (!tempPtr)throw EXCEPTION(EX_INIT);
						else
						{
							basePtr = tempPtr;
							for (size_t count = next_pos; count > index; --count)basePtr[count] = basePtr[count - 1];
							basePtr[index] = *element;
							++next_pos;
							return;
						}
					}
				}
			}
		}

		void erase(size_t index)
		{
			if (!basePtr)throw EXCEPTION(EX_PTR);
			else
			{
				if (index < 0 || index >= next_pos)return;
				else
				{
					for (size_t count = index; count < next_pos - 1; ++count)basePtr[count] = basePtr[count + 1];
					--next_pos;
				}
			}
		}
	};

	class RINGHELP_API PROTON
	{
	protected:
		float _width{ 1.0f };
		float _height{ 1.0f };

		bool hor_dir{ false };
		bool ver_dir{ false };
		float move_sx{ 0 };
		float move_sy{ 0 };
		float move_ex{ 0 };
		float move_ey{ 0 };
		float slope{ 0 };
		float intercept{ 0 };

		float _speed{ 1.0f };

		D2D1_RECT_F unit_rect{};

	public:
		D2D1_POINT_2F start{};
		D2D1_POINT_2F end{};
		D2D1_POINT_2F center{};

		float x_rad{ 0 };
		float y_rad{ 0 };

		PROTON();
		PROTON(float _sx, float _sy);
		PROTON(float _sx, float _sy, float _first_width, float _first_height);

		virtual ~PROTON() {};

		void set_edges();
		void new_dims(float _new_width, float _new_height);

		void new_width(float _new_width);
		void new_height(float _new_height);

		void set_path(float _ex, float _ey);

		float get_width()const;
		float get_height()const;

		float get_target_x()const;
		float get_target_y()const;

		D2D1_RECT_F get_rect() const;
	};

	class RINGHELP_API FIELD :public PROTON
	{
	private:
		int frame = 0;
		int max_frames = 0;
		int frame_delay = 1;

		FIELD(fields _what, float s_x, float s_y);

	public:
		fields type;
		
		bool move(float gear, dirs to_where);

		int get_frame();

		void Release();

		static FIELD* create(fields what, float sx, float sy);
	};

	class RINGHELP_API HERO :public PROTON
	{
	private:
		int frame = 0;
		int max_frames = 5;
		int frame_delay = 12;
		int max_frame_delay = 12;

		bool in_jump = false;
		bool jump_up = false;

		HERO(float s_x, float s_y);

	public:
		dirs dir{ dirs::right };
		actions action{ actions::stop };
		
		int lifes = 100;
		int damage = 5;
		int armor = 1;

		void climb(float ground_speed, D2D1_RECT_F hill, fields hill_type);

		void jump(BAG<D2D1_RECT_F>& grounds);

		void fall(BAG<D2D1_RECT_F>& grounds);

		int get_frame();

		void Release();

		static HERO* create(float sx, float sy);
	};

	class RINGHELP_API EVIL :public PROTON
	{
	private:
		int frame = 0;
		int max_frames = 5;
		int frame_delay = 12;
		int max_frame_delay = 12;

		int attack_delay{ 0 };
		int max_attack_delay{ 0 };

		EVIL(creatures _what_type, float s_x, float s_y);

	public:
		creatures type;
		dirs dir{ dirs::left };
		actions action{ actions::run };

		int lifes = 100;
		int damage = 5;
		int armor = 1;

		bool move(float gear, BAG<FIELD>& grounds);

		int attack();

		int get_frame();

		void Release();

		static EVIL* create(creatures what_type, float sx, float sy);
	};

	// FUNCTIONS **********************************************

	RINGHELP_API float distance(D2D1_POINT_2F first, D2D1_POINT_2F second);

	RINGHELP_API bool intersect(D2D1_RECT_F first, D2D1_RECT_F second);
	RINGHELP_API bool intersect(D2D1_POINT_2F first_center, D2D1_POINT_2F second_center, float first_xrad, float second_xrad,
		float first_yrad, float second_yrad);

	RINGHELP_API void sort(BAG<D2D1_POINT_2F>& bag, D2D1_POINT_2F ref);
	RINGHELP_API void sort(BAG<D2D1_RECT_F>& bag, D2D1_RECT_F ref);
}