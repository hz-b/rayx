# Testing in RAY-X

There are several kinds of tests: 
## Testing C++ Code
testing only c++ code and not using the shader, to check if parameters of optical elements are calculated correctly

check especially if the values that are derived from given user parameters and given to the shader are correct. These include the surface Parameters, the object parameters, the element parameters and the world to element and element to world coordinate transformation matrices, each of which are stored in an 16 element value array. 

## Testing Shader Code (test_shader.cpp) 

The testing suite "Tracer" contains unit tests that check if the individual functions in the shader code are behaving as expected. As the functions that are tested are on the shader and in our current framework the only values that can be moved to the shader are mainly the Ray and Optical Element buffers, we cannot just call the functions with the required input values. 
Instead we first store the test values in Rays on the C++ side and retrieve them from the Rays on the shader side. Then the test can be executed on the shader and the results are stored again in the Ray buffer (outputRays). Back on the C++ side this can then be compared with the expected values. 

Example: Testing the refraction function <br>
The refraction function on the shader calculates the direction and weight of the refracted ray from the direction of the incoming ray, the normal at the intersection and the line density. The input to the test should therefore be:
- dvec3 **direction**
- dvec3 **normal**
- double **lineDensity**
- double **weight**

The output and therefore the values to verify are:
- dvec3 **direction_out**
- double **weight_out**

after the refraction

To move the test data to the shader and retrieve the results after applying the function we use the Ray buffer. Each test case is encoded in one Ray and we can add as many test cases as we want to the ray buffer. A ray consists of:

- dvec3 **position**
- dvec3 **direction**
- double **energy**
- double **weight**
- dvec4 **stokes**
- double **order**
- double **pathLength**
- double **lastElement**
- double **extra Parameter**

We can for example encode the test values for the refraction test as:
- dvec **position** $\leftarrow$ **normal**
- dvec **direction** $\leftarrow$ **direction**
- double **energy** $\leftarrow$ **lineDensity**
- double **weight** $\leftarrow$ **weight**
- others $\leftarrow$ 0

For this, we can use the function "addTestSetting" that receives the test values in the correct order, creates a ray and adds it to a given ray vector which is in this case the one that will be transferred to the shader (std::vector\<RAYX::Ray\> **testValues**).
To be able to verify the result that we will later retrieve from the shader, we need to store also the expected direction_out and weight_out. To make the comparison later easier we also store these in a Ray that corresponds to the test case and add it to std::vector\<RAYX::Ray\> **correct** using for example the following encoding:
- dvec **position** $\leftarrow$ (0,0,0)
- dvec **direction** $\leftarrow$ **direction_expected**
- double **energy** $\leftarrow$ 0
- double **weight** $\leftarrow$ **weight_expected**
- others $\leftarrow$ 0

Now we have in both vectors one Ray for each test case, where **testValues** contains the values that we move to the shader and **correct** contains those that we expect to get back. Now, we can move **testValues** as the Ray buffer to the shader.

Then, on the shader side we need to make sure that the test values are "unpacked" correctly from the ray buffer. We can execute the function and store the updated direction and weight in the output ray buffer in the same format as they are stored in **correct** on the C++ side which is:
- dvec **position** $\leftarrow$ (0,0,0)
- dvec **direction** $\leftarrow$ **direction_out**
- double **energy** $\leftarrow$ 0
- double **weight** $\leftarrow$ **weight_out**
- others $\leftarrow$ 0

These Rays are returned to the C++ test code as **outputRays** where the can be compared with compareFromCorrect(correct, outputRays, tolerance); for a given tolerance.

For other functions, like the approximation of sinus for example, we do not necessarily need the **correct** vector but we can simply apply the function sin() to the **testValues** and compare them directly with: <br>
auto sinfun = fn<double, double>([](double x) { return sin(x); }); <br>
compareFromFunction(sinfun, testValues, outputRays, tolerance);

If a test has more test values than a ray has paramters, one could use the opticalElement buffer and add e.g. one opticalElement for each test case.

To make things even more complicated, we also need to have an id for each test to distinguish on the shader side which test is run and how the rays should be interpreted. The id is set in the surfaceParams of an optical Element. Because we want to have only one main function on the shader we also have to distinguish between a test case and a normal run of a beamline. This is achieved by setting the id to 0 if it is a beamline and to the test id otherwise (there is no test with id=0).

## Testing Beamlines (test_shader.cpp) 
Tests from the testing suite opticalElements read a beamline from a given rml file, run the tracer on it and writes the returned rays to a csv file with the same name as the rml file. 
If the beamlines give deterministic results, we can compare them with the output of RAY-UI using the test.py file. Therefore it is necessary to export the traced data from the same beamline traced in RAY-UI. Moreover, the beamline needs to end with an image plane because of the different coordinate systems that are used (Ray coodinates vs world coordinates).