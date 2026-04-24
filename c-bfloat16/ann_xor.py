import numpy as np

# Initializing Inputs and Outputs
x1 = np.array([[0,0],[0,1],[1,0],[1,1]]) # Inputs
y1 = np.array([[0], [1], [1], [0]]) # Outputs

# Defining the sigmoid activation function
def sigmoid(z):
    z = 1/(1 + np.exp(-z))
    return z

# Defining the sigmoid activation function
def sigmoid_appr(z):
    z = 1.0/2.0*(z/(1+abs(z))+1)
    return z

# Initializing learning rate and number of iterations
l_rate = 0.5
iterations = 100000

# Initializing Layers
n_input = 2
n_hidden = 2
n_output = 1 

# Assigning initial random Weights and Biases at the hidden layer
hidden_layer_wt = np.random.rand(n_input, n_hidden) 
hidden_layer_bias = np.random.rand(n_hidden)

# Assigning initial random Weights and Biases at the output layer
output_layer_wt  = np.random.rand(n_hidden, n_output)
output_layer_bias = np.random.rand(n_output)

for i in range(iterations):
    # Forward propagation
    h_layer_int = np.dot(x1, hidden_layer_wt)
    h_layer_int = h_layer_int + hidden_layer_bias
    h_layer_output = sigmoid(h_layer_int)
    
    o_layer_int = np.dot(h_layer_output, output_layer_wt)
    o_layer_int = o_layer_int + output_layer_bias
    o_layer_output = sigmoid(o_layer_int)
    
    # Backpropagation
    error = (y1 - o_layer_output)
    delta_output = error * (o_layer_output * (1 - o_layer_output))
    
    error_hlayer = np.dot(delta_output, output_layer_wt.T)
    delta_hidden = error_hlayer * (h_layer_output * (1 - h_layer_output))
    
    # Updating weights and biases
    output_layer_wt = output_layer_wt + np.dot(h_layer_output.T, delta_output) * l_rate
    output_layer_bias = output_layer_bias + np.sum(delta_output) * l_rate
    hidden_layer_wt = hidden_layer_wt + np.dot(x1.T, delta_hidden) * l_rate
    hidden_layer_bias = hidden_layer_bias + np.sum(delta_hidden) * l_rate

    # Displaying final weights, biases, and the output from the neural network after specified iterations

    # Forward propagation
    h_layer_int = np.dot(x1, hidden_layer_wt)
    h_layer_int = h_layer_int + hidden_layer_bias
    h_layer_output = sigmoid_appr(h_layer_int)
    
    o_layer_int = np.dot(h_layer_output, output_layer_wt)
    o_layer_int = o_layer_int + output_layer_bias
    o_layer_output = sigmoid_appr(o_layer_int)

print("Final hidden weights: ", end='')
print(*hidden_layer_wt)
print("Final hidden layer bias: ", end='')
print(*hidden_layer_bias)

print("Final output weights: ", end='')
print(*output_layer_wt)
print("Final output bias: ", end='')
print(*output_layer_bias)

print("\\nOutput from neural network after", iterations, "Iterations: ", end='')
print(*o_layer_output)