# ParameterFunction

This programs is about getting the value of $f(x) \in [-1, 1]$, so that
$$
  x = x_0 - \sqrt{\pi m_i / m_e}
    \int\limits_1^{f(x)} \frac{dz}{\sqrt{\pi/2 - \arcsin(z) - z \sqrt{1 - z^2}}}\,,
$$
where $m_i$ and $m_e$ is the mass of ions and electrons.

First, run `evaluate_function.py` with the expected parameters.
Function parameters such as $x_0$, $x_{max}$, $dx$, $m_i/m_e$ along
with function key-value pairs will be stored in the .bin file.
The function can then be read by the C++ class described in
`parameter_function.hpp`. Pass the resulting file to the
constructor and the function can be used via the get_value method
for all $x \in [x_0,\, x_{max}]$. Runtime table function change can
be achieved with evaluate_from_file method, previous data will be lost.

Use case showed in `example.cpp`, magnetic field in the transition
layer is computed for $x_0 = 20.0,~dx = 0.05$ and $~m_i/m_e = 400.0.$