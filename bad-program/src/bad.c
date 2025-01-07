int main(int argc, char **argv)
{
	volatile int a[5] = {0};
	a[6] = argc;
	return 0;
}
