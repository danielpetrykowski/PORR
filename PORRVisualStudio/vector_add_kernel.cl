__kernel void loop(int numberVertics, uint2 random, __global int* citis, __global int* graph, __global uint2* C, __global int* citiesOrder) {
	int position1=0,position2=0;
	int id = get_global_id(0);
	
	
	uint2 result;
	//Random 1
	uint seed = random.x + id;
	uint t = seed ^ (seed << 11);  
	result.x = random.y ^ (random.y >> 19) ^ (t ^ (t >> 8));
	random = result.x;
	
	
	//Random 2
	seed = random.x + id;
	t = seed ^ (seed << 11);
	result.y = random.y ^ (random.y >> 19) ^ (t ^ (t >> 8));
	random = result.y;
	
	position1 = result.x % numberVertics;
	position2 = result.y % numberVertics;	
	
	
	//Swap
	for(int i= 0; i<numberVertics;i++){
		if(i==position1){
			citiesOrder[i + id*numberVertics] = citis[position2];
		} else if (i==position2){
			citiesOrder[i + id*numberVertics] = citis[position1];
		} else {
			citiesOrder[i + id*numberVertics] = citis[i];
		}	
	} 
	
	
	//Calculate distance
	int distance = 0;
	int start = citiesOrder[0 + id*numberVertics];
	int current = start;
	int next;
	for(int i=1 + id*numberVertics;i<id*numberVertics + numberVertics;i++){
		//printf("%d\n", i);
		//printf("%d -> ", current);
		next = citiesOrder[i];
		distance = distance + graph[current*numberVertics + next];
		//printf("%d -> ", distance);
		current = next;
	}
	//printf("%d\n", current);
	distance += graph[current*numberVertics + start];
	
		
	C[id] = distance; // A[id] * k;
}