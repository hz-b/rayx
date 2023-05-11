# User vs Model Parameter

In rayx we intend to work on better modularity and readability in our code. In our opinion, one step to achieve this, to reevaluate and restructure the parameters of Ray-UI. Since a lot of the parameters had dependencies on each other, we decided that a distinction between them was needed. This is where our definition of User and Model Parameters comes into play. 

## Model Parameter

A Model Parameter is any parameter that is directly influencing the calculations of the tracer. Further, a Model Parameter can also be a parameter, which is required for the fundamental definition of our model. 

### Example

The position and direction Matrix are not directly used in the calculations of the Tracer, but they still are Model Parameters, since they are essential in directly defining our model.


## User Parameters

User Parameters are all the parameters that are only used to calculate Model Parameters. They only exist to improve the user experience of our application and are neither passed directly to the tracer nor are needed to define our model.