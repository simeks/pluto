namespace python
{
	template<typename ... TArgs>
	Tuple make_tuple(TArgs... args)
	{
		const size_t n = sizeof...(TArgs);
		// Convert arguments
		std::array<Object, n> conv = {
			to_python<TArgs>(args)...
		};

		Tuple t(n);
		for (size_t i = 0; i < n; ++i)
			t.set(i, conv[i]);
		return t;
	}
}
