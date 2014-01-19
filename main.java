

// --------- What is the magic box? - how does it work (DEFINING) -------
int subtract(int u, int p)
{
	return u - p;
}

// -------------USING/CALLING it--------------------

x = subtract(5, 2) + 9;


int add(int a, int b)
{
	return a + b;
}

void print(string s)
{
	// Draws s to the screen
	// takes the s figures out the shape of the letters and where to draw the pixels
}

void sleep(int x)
{
	// does something, doesn't return a value
}

void f(x)
{
	int y;
	y = x;
}

int n = add(2, 3);

// Function definition	
int add(int x, int y)
{
	return x + y;
}

// Using 
add(5, 2) + add(7, 9)

f(x) = x + y;

// Define a function that multiples x, y

int multiply(int x, int y)
{
	return x * y;
}

// Use the function, multiply 9 * 3 + 2
int a = multiply(9, 3) + 2;	

// Make a function that returns (x + y) / z
// and call the function foo
float foo(float x, float y, float z)
{
	return (x + y) / z;
}
	
// Use the function and store the return value in a variable called result
//2,9,2

float result = add(foo(2, 9, 2), 2);

// get the result of foo + 2 (use the function add NOT the operator)
int add(int x, int y)
{
	return x + y;
}

// Add 5 + 3 + 8 + 9

add(add(add(5, 3), 8), 9)


// Make an array of 3 boxes and assign 3, 8, 6 to 
//them in the first, second, third box

int x = 3;
int y = 8;
int z = 6;


int jay[3];
jay[0] = 3;
jay[1] = 8;
jay[2] = 6;

// 

float result = 0;

for (int i = 0; i < 100; i++)
{
	result = result + jay[i];
}

EQUIVALENT



// Make a for loop that runs 20 times

for (int i = 0; i < 20; i++)
{
}

// Make a for loop that runs 20 times but starts with i = 70
// i = 70..50

for (int i = 70; i > 50; i--)
{
}

int i = 70;
while(i > 50)
{
	i--;
}

result = 3
result = 11

// if statement
// make a piecewise function 
jay(x) = { 
		 1 if x > 0, 
		 0 if x < 0 
	   }

// Define function
int jay(int x)
{
		if(x > 0)
		{
			return 1;
		}
		else if(x < 0)
		{
			return 0;
		}
		else
		{
			return 0;
		}
}
	
// use function

int x = jay(5);
int y = jay(-2);

if (jay(5) == 0)
{
	System.out.println("It's NOT working");
}

void pause(string s)
{
	// makes the system wait
}

//-----------------------------------------

int main()
{
	//
	return 0;
}