/* 
 * File:   GradientCalculator.hpp
 * Author: matthewsupernaw
 *
 * Created on February 13, 2014, 2:10 PM
 */

#ifndef GRADIENTCALCULATOR_HPP
#define	GRADIENTCALCULATOR_HPP

#include "ADNumber.hpp"

#ifdef AD_OPENCL_SUPPORT
#include "support/cl.hpp"

inline std::ostream& operator<<(std::ostream& out, const cl::Platform& platform) {
    out << "CL_PLATFORM_PROFILE    = " << platform.getInfo<CL_PLATFORM_PROFILE > () << "\n";
    out << "CL_PLATFORM_VERSION    = " << platform.getInfo<CL_PLATFORM_VERSION > () << "\n";
    out << "CL_PLATFORM_NAME       = " << platform.getInfo<CL_PLATFORM_NAME > () << "\n";
    out << "CL_PLATFORM_VENDOR     = " << platform.getInfo<CL_PLATFORM_VENDOR > () << "\n";
    out << "CL_PLATFORM_EXTENSIONS = " << platform.getInfo<CL_PLATFORM_EXTENSIONS > () << "\n";
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const cl::Device& device) {
    out << "CL_DEVICE_ADDRESS_BITS                  = " << device.getInfo<CL_DEVICE_ADDRESS_BITS > () << "\n";
    out << "CL_DEVICE_AVAILABLE                     = " << device.getInfo<CL_DEVICE_AVAILABLE > () << "\n";
    out << "CL_DEVICE_COMPILER_AVAILABLE            = " << device.getInfo<CL_DEVICE_COMPILER_AVAILABLE > () << "\n";
    out << "CL_DEVICE_ENDIAN_LITTLE                 = " << device.getInfo<CL_DEVICE_ENDIAN_LITTLE > () << "\n";
    out << "CL_DEVICE_ERROR_CORRECTION_SUPPORT      = " << device.getInfo<CL_DEVICE_ERROR_CORRECTION_SUPPORT > () << "\n";
    out << "CL_DEVICE_EXECUTION_CAPABILITIES        = " << device.getInfo<CL_DEVICE_EXECUTION_CAPABILITIES > () << "\n";
    out << "CL_DEVICE_EXTENSIONS                    = " << device.getInfo<CL_DEVICE_EXTENSIONS > () << "\n";
    out << "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE         = " << device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE > () << "\n";
    out << "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE         = " << device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE > () << "\n";
    out << "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE     = " << device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE > () << "\n";
    out << "CL_DEVICE_GLOBAL_MEM_SIZE               = " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE > () << "\n";
    out << "CL_DEVICE_IMAGE_SUPPORT                 = " << device.getInfo<CL_DEVICE_IMAGE_SUPPORT > () << "\n";
    out << "CL_DEVICE_IMAGE2D_MAX_HEIGHT            = " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_HEIGHT > () << "\n";
    out << "CL_DEVICE_IMAGE2D_MAX_WIDTH             = " << device.getInfo<CL_DEVICE_IMAGE2D_MAX_WIDTH > () << "\n";
    out << "CL_DEVICE_IMAGE3D_MAX_DEPTH             = " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_DEPTH > () << "\n";
    out << "CL_DEVICE_IMAGE3D_MAX_HEIGHT            = " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_HEIGHT > () << "\n";
    out << "CL_DEVICE_IMAGE3D_MAX_WIDTH             = " << device.getInfo<CL_DEVICE_IMAGE3D_MAX_WIDTH > () << "\n";
    out << "CL_DEVICE_LOCAL_MEM_SIZE                = " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE > () << "\n";
    out << "CL_DEVICE_LOCAL_MEM_TYPE                = " << device.getInfo<CL_DEVICE_LOCAL_MEM_TYPE > () << "\n";
    out << "CL_DEVICE_MAX_CLOCK_FREQUENCY           = " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY > () << "\n";
    out << "CL_DEVICE_MAX_COMPUTE_UNITS             = " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS > () << "\n";
    out << "CL_DEVICE_MAX_CONSTANT_ARGS             = " << device.getInfo<CL_DEVICE_MAX_CONSTANT_ARGS > () << "\n";
    out << "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE      = " << device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE > () << "\n";
    out << "CL_DEVICE_MAX_MEM_ALLOC_SIZE            = " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE > () << "\n";
    out << "CL_DEVICE_MAX_PARAMETER_SIZE            = " << device.getInfo<CL_DEVICE_MAX_PARAMETER_SIZE > () << "\n";
    out << "CL_DEVICE_MAX_READ_IMAGE_ARGS           = " << device.getInfo<CL_DEVICE_MAX_READ_IMAGE_ARGS > () << "\n";
    out << "CL_DEVICE_MAX_SAMPLERS                  = " << device.getInfo<CL_DEVICE_MAX_SAMPLERS > () << "\n";
    out << "CL_DEVICE_MAX_WORK_GROUP_SIZE           = " << device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE > () << "\n";
    out << "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS      = " << device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS > () << "\n";
    out << "CL_DEVICE_MAX_WRITE_IMAGE_ARGS          = " << device.getInfo<CL_DEVICE_MAX_WRITE_IMAGE_ARGS > () << "\n";
    out << "CL_DEVICE_MEM_BASE_ADDR_ALIGN           = " << device.getInfo<CL_DEVICE_MEM_BASE_ADDR_ALIGN > () << "\n";
    out << "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE      = " << device.getInfo<CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE > () << "\n";
    out << "CL_DEVICE_NAME                          = " << device.getInfo<CL_DEVICE_NAME > () << "\n";
    out << "CL_DEVICE_PLATFORM                      = " << device.getInfo<CL_DEVICE_PLATFORM > () << "\n";
    out << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR   = " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR > () << "\n";
    out << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE = " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE > () << "\n";
    out << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT  = " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT > () << "\n";
    out << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT    = " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT > () << "\n";
    out << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG   = " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG > () << "\n";
    out << "CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT  = " << device.getInfo<CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT > () << "\n";
    out << "CL_DEVICE_PROFILE                       = " << device.getInfo<CL_DEVICE_PROFILE > () << "\n";
    out << "CL_DEVICE_PROFILING_TIMER_RESOLUTION    = " << device.getInfo<CL_DEVICE_PROFILING_TIMER_RESOLUTION > () << "\n";
    out << "CL_DEVICE_QUEUE_PROPERTIES              = " << device.getInfo<CL_DEVICE_QUEUE_PROPERTIES > () << "\n";
    out << "CL_DEVICE_SINGLE_FP_CONFIG              = " << device.getInfo<CL_DEVICE_SINGLE_FP_CONFIG > () << "\n";
    out << "CL_DEVICE_TYPE                          = " << device.getInfo<CL_DEVICE_TYPE > () << "\n";
    out << "CL_DEVICE_VENDOR_ID                     = " << device.getInfo<CL_DEVICE_VENDOR_ID > () << "\n";
    out << "CL_DEVICE_VENDOR                        = " << device.getInfo<CL_DEVICE_VENDOR > () << "\n";
    out << "CL_DEVICE_VERSION                       = " << device.getInfo<CL_DEVICE_VERSION > () << "\n";
    out << "CL_DRIVER_VERSION                       = " << device.getInfo<CL_DRIVER_VERSION > () << "\n";
    return out;
}

#endif

template<class T>
class GradientCalculator {
    //ad::ArrayBasedExpression<T> array;

    struct CExp {
        int id;
        int op;
        //        int left;
        //        int right;
        T value;
    };
    
#ifdef AD_OPENCL_SUPPORT
    std::string opencl_kernel;

    cl::CommandQueue queue;
    cl::Kernel kernel;
    cl::Context context;
    cl::Program program_;
    std::vector<cl::Device> devices;


    cl::Buffer d_id; // = cl::Buffer(context, CL_MEM_READ_ONLY, array.size * sizeof (int));
    cl::Buffer d_op; // = cl::Buffer(context, CL_MEM_READ_ONLY, array.size * sizeof (int));
    cl::Buffer d_value; // = cl::Buffer(context, CL_MEM_READ_ONLY, array.size * sizeof (HOST_TYPE));
    cl::Buffer d_left; // = cl::Buffer(context, CL_MEM_READ_ONLY, array.size * sizeof (int));
    cl::Buffer d_right; // = cl::Buffer(context, CL_MEM_READ_ONLY, array.size * sizeof (int));
    cl::Buffer d_parameters; // = cl::Buffer(context, CL_MEM_READ_ONLY, parameters.size() * sizeof (int));
    cl::Buffer d_gradient; // = cl::Buffer(context, CL_MEM_WRITE_ONLY, parameters.size() * sizeof (HOST_TYPE));
    cl::Buffer d_stack; // = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameters.size() * sizeof (int));
    cl::Buffer d_search_stack; // = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameters.size() * sizeof (int));
    cl::Buffer d_value_stack; // = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameters.size() * sizeof (HOST_TYPE));
    cl::Buffer d_derivative_stack; // = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameters.size() * sizeof (HOST_TYPE));

    cl::Buffer d_exp;

    int* h_id;
    int* h_op;
    T* h_value;
    int* h_left;
    int* h_right;
    int* h_parameters;




    bool gpu_initialized;
    unsigned int expression_size;
    unsigned int stack_size;
    unsigned int gpu_max_memory;
    unsigned int gpu_max_compute_units;
    unsigned int gpu_memory_utilzed;
    unsigned int gpu_compute_units_utilized;
    unsigned int gpu_stride;
#endif

    std::vector<int> stack;
    std::vector<int> search_stack;
    std::vector<T> value_stack;
    std::vector<T> derivative_stack;

    std::vector<T> values; //(expression.size());
    std::vector<T> derivatives; //(expression.size());

    T max_gradient_component;

public:

    GradientCalculator():
#ifdef AD_OPENCL_SUPPORT
     gpu_initialized(false),
    expression_size(0),
    stack_size(0)
#endif
     max_gradient_component(0){

    }

    const T GetMaxGradientComponent(){
        return this->max_gradient_component;
    }
#ifdef AD_OPENCL_SUPPORT
    void GradientGPU(const ad::ADNumber<T> &f,
            std::vector<int> &parameter_ids,
            std::vector<T> &gradient) {



//        std::cout < "gpu....\n";
        //check if the problem changed

        if (!this->gpu_initialized) {

            std::ifstream in;
            in.open("grad_calc4.cl");


            std::stringstream ss;
            std::string line = "";

            while (in.good()) {
                std::getline(in, line);
                ss << line << "\n";
            }

            this->opencl_kernel = ss.str();


            this->expression_size = 0;
            cl_int err = CL_SUCCESS;
            try {

                // Query platforms
                std::vector<cl::Platform> platforms;

                cl::Platform::get(&platforms);
                if (platforms.size() == 0) {
                    std::cout << "Platform size 0\n";
                    exit(0);
                }


                std::cout << platforms[0];

                // Get list of devices on default platform and create context
                cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
                context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
                devices = context.getInfo<CL_CONTEXT_DEVICES > ();
                //std::cout<<devices[0]<<"\n";

                const cl::Device device = devices[0];
                this->gpu_max_memory = device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE > (NULL);



                // Create command queue for first device
                this->queue = cl::CommandQueue(context, devices[0], 0, &err);



                //Build kernel from source string
                cl::Program::Sources source(1,
                        std::make_pair(this->opencl_kernel.c_str(), this->opencl_kernel.size()));
                program_ = cl::Program(context, source);
                program_.build(devices);

                // Create kernel object
                kernel = cl::Kernel(program_, "gradient", &err);



            } catch (cl::Error err) {
                std::cerr
                        << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
                std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
                exit(0);
            }
            this->gpu_initialized = true;
        }

        ad::PostOrderIterator<T> it(f.GetExpression());
        std::vector<CExp> cexp;
        while (it) {

            ad::Expression<T> * exp = (ad::Expression<T>*)it;
            CExp e;
            e.id = exp->GetId();
            e.op = exp->GetOp();
            e.value = exp->GetValue();
            cexp.push_back(e);
            it++;
        }

        try {

            d_exp = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, cexp.size() * sizeof (CExp), cexp.data(), 0);
            d_parameters = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, parameter_ids.size() * sizeof (int), parameter_ids.data(), 0);
            d_gradient = cl::Buffer(context, CL_MEM_WRITE_ONLY, parameter_ids.size() * sizeof (HOST_TYPE));
            d_value_stack = cl::Buffer(context, CL_MEM_READ_WRITE, cexp.size() * parameter_ids.size() * sizeof (HOST_TYPE));
            d_derivative_stack = cl::Buffer(context, CL_MEM_READ_WRITE, cexp.size() * parameter_ids.size() * sizeof (HOST_TYPE));

        } catch (cl::Error err) {
            std::cerr
                    << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
            std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
            exit(0);
        }
        //  }

        try {

            queue.enqueueMapBuffer(d_exp, CL_FALSE, CL_MAP_READ, 0, cexp.size() * sizeof (int));
            queue.enqueueMapBuffer(d_parameters, CL_FALSE, CL_MAP_READ, 0, parameter_ids.size() * sizeof (int));


            kernel.setArg(0, d_exp);
            kernel.setArg(1, cexp.size());
            kernel.setArg(2, d_parameters);
            kernel.setArg(3, d_gradient);
            kernel.setArg(4, parameter_ids.size());
            kernel.setArg(5, d_value_stack); // cexp.size()  * sizeof (float), NULL);
            kernel.setArg(6, d_derivative_stack); //cexp.size()* sizeof (float), NULL); 


            unsigned int n = parameter_ids.size();
            // Number of work items in each local work group
            cl::NDRange localSize(1);
            // Number of total work items - localSize must be devisor
            cl::NDRange globalSize(n); //(int) (ceil(n / (HOST_TYPE) 64)*64));

            cl::Event event;
            queue.enqueueNDRangeKernel(
                    kernel,
                    cl::NullRange,
                    globalSize,
                    localSize,
                    NULL,
                    &event);

            // Block until kernel completion
            event.wait();

            queue.enqueueReadBuffer(d_gradient, CL_TRUE, 0, parameter_ids.size() * sizeof (HOST_TYPE), gradient.data());


        } catch (cl::Error err) {
            std::cerr
                    << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
            std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
            exit(0);
        }


    }

    void GradientGPU3(const ad::ADNumber<T> &f,
            std::vector<int> &parameter_ids,
            std::vector<T> &gradient) {



//        std::cout < "gpu....\n";
        //check if the problem changed

        if (!this->gpu_initialized) {

            std::ifstream in;
            in.open("grad_calc3.cl");


            std::stringstream ss;
            std::string line = "";

            while (in.good()) {
                std::getline(in, line);
                ss << line << "\n";
            }

            this->opencl_kernel = ss.str();


            this->expression_size = 0;
            cl_int err = CL_SUCCESS;
            try {

                // Query platforms
                std::vector<cl::Platform> platforms;

                cl::Platform::get(&platforms);
                if (platforms.size() == 0) {
                    std::cout << "Platform size 0\n";
                    exit(0);
                }


                std::cout << platforms[0];

                // Get list of devices on default platform and create context
                cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
                context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
                devices = context.getInfo<CL_CONTEXT_DEVICES > ();
                //std::cout<<devices[0]<<"\n";

                const cl::Device device = devices[0];
                this->gpu_max_memory = device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE > (NULL);



                // Create command queue for first device
                this->queue = cl::CommandQueue(context, devices[0], 0, &err);



                //Build kernel from source string
                cl::Program::Sources source(1,
                        std::make_pair(this->opencl_kernel.c_str(), this->opencl_kernel.size()));
                program_ = cl::Program(context, source);
                program_.build(devices);

                // Create kernel object
                kernel = cl::Kernel(program_, "gradient", &err);



            } catch (cl::Error err) {
                std::cerr
                        << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
                std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
                exit(0);
            }
            this->gpu_initialized = true;
        }

        ad::PostOrderIterator<T> it(f.GetExpression());
        std::vector<CExp> cexp;
        while (it) {

            ad::Expression<T> * exp = (ad::Expression<T>*)it;
            CExp e;
            e.id = exp->GetId();
            e.op = exp->GetOp();
            e.value = exp->GetValue();
            cexp.push_back(e);

            it++;
        }



        size_t problem_size = array.Bytes() + parameter_ids.size()*(sizeof (int) *2 + sizeof (T)*2);

        // std::cout<<"using "<<problem_size<<" of "<<this->gpu_max_memory<<"("<<((float)problem_size/this->gpu_max_memory)<<"%) gpu memory\n";
        if ((problem_size) >= this->gpu_max_memory) {
            std::cout << "problem too big...." << std::endl;
            exit(0);
        }
        //        std::cout<<"-->"<<parameter_ids.size()*(sizeof (int) *2 + sizeof (T)*2);
        //           exit(0);
        size_t active_kernels = (this->gpu_max_memory / problem_size) - 1;
        //std::cout << "active kernels = " << active_kernels << "\n";

        //if (this->expression_size != array.size) {//reallocate
        try {



            //this->h_parameters = std::vector<int>(active_kernels * array.size);
            ////std::cout << __LINE__ << std::endl;
            d_exp = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, cexp.size() * sizeof (CExp), cexp.data(), 0);
            ////std::cout << __LINE__ << std::endl;
            //            d_op = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.op.size() * sizeof (int), h_op, 0);
            //            ////std::cout << __LINE__ << std::endl;
            //            d_value = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.value.size() * sizeof (HOST_TYPE), h_value, 0);
            //            ////std::cout << __LINE__ << std::endl;
            //            d_left = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.left.size() * sizeof (int), h_left, 0);
            //            ////std::cout << __LINE__ << std::endl;
            //            d_right = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.right.size() * sizeof (int), h_right, 0);
            //            ////std::cout << __LINE__ << std::endl;
            d_parameters = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, parameter_ids.size() * sizeof (int), parameter_ids.data(), 0);
            //            ////std::cout << __LINE__ << std::endl;
            d_gradient = cl::Buffer(context, CL_MEM_WRITE_ONLY, parameter_ids.size() * sizeof (HOST_TYPE));
            //            ////std::cout << __LINE__ << std::endl;
            //            d_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (int));
            //            ////std::cout << __LINE__ << std::endl;
            //            d_search_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (int));
            //            ////std::cout << __LINE__ << std::endl;
            //            d_value_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (HOST_TYPE));
            //            ////std::cout << __LINE__ << std::endl;
            //            d_derivative_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (HOST_TYPE));

        } catch (cl::Error err) {
            std::cerr
                    << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
            std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
            exit(0);
        }
        //  }

        try {
            //            this->expression_size = array.size;
            //            this->h_id = array.id.data();
            //            this->h_op = array.op.data();
            //            this->h_value = array.value.data();
            //            this->h_left = array.left.data();
            //            this->h_right = array.right.data();
            //            this->h_parameters = parameter_ids.data();
            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_exp, CL_FALSE, CL_MAP_READ, 0, cexp.size() * sizeof (int));
            //            //std::cout << __LINE__ << std::endl;
            //            queue.enqueueMapBuffer(d_op, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (int));
            //            //std::cout << __LINE__ << std::endl;
            //            queue.enqueueMapBuffer(d_value, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (HOST_TYPE));
            //            //std::cout << __LINE__ << std::endl;
            //            queue.enqueueMapBuffer(d_left, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (int));
            //            //std::cout << __LINE__ << std::endl;
            //            queue.enqueueMapBuffer(d_right, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (int));
            //            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_parameters, CL_FALSE, CL_MAP_READ, 0, parameter_ids.size() * sizeof (int));
            //     queue.enqueueMapBuffer(d_gradient, CL_TRUE, CL_MAP_WRITE, 0, parameter_ids.size() * sizeof (HOST_TYPE));

            kernel.setArg(0, d_exp);
            kernel.setArg(1, cexp.size());
            kernel.setArg(2, d_parameters);
            kernel.setArg(3, d_gradient);
            kernel.setArg(4, parameter_ids.size());
            kernel.setArg(5, cexp.size() * sizeof (float), NULL);
            kernel.setArg(6, cexp.size() * sizeof (float), NULL);


            unsigned int n = parameter_ids.size();
            // Number of work items in each local work group
            cl::NDRange localSize(1);
            // Number of total work items - localSize must be devisor
            cl::NDRange globalSize((int) (ceil(n / (HOST_TYPE) 64)*64));
            // std::cout<<(int) (ceil(n / (HOST_TYPE) 64)*64)<<"\n";
            //std::cout << "launching opencl kernels....\n";
            // Enqueue kernel
            cl::Event event;
            queue.enqueueNDRangeKernel(
                    kernel,
                    cl::NullRange,
                    globalSize,
                    localSize,
                    NULL,
                    &event);

            // Block until kernel completion
            event.wait();
            //            cl_ulong start =
            //                    event.getProfilingInfo<CL_PROFILING_COMMAND_START > ();
            //            cl_ulong end =
            //                    event.getProfilingInfo<CL_PROFILING_COMMAND_END > ();
            //            double time = 1.e-9 * (end - start);
            //            std::cout << "Time for kernel to execute " << time << std::endl;

            //float* gg = new float[parameter_ids.size()];
            // Read back d_c
            queue.enqueueReadBuffer(d_gradient, CL_TRUE, 0, parameter_ids.size() * sizeof (HOST_TYPE), gradient.data());
            //            //
            //                    for (int i = 0; i < n; i++) {
            //                        gradient[i] = gg[i];
            //                        // std::cout<<gg[i] << " ";
            //                        //this->gradient_m[i] = gradient[i];
            //                    }
        } catch (cl::Error err) {
            std::cerr
                    << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
            std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
            exit(0);
        }


    }

    void GradientGPU2(const ad::ADNumber<T> &f,
            std::vector<int> &parameter_ids,
            std::vector<T> &gradient) {


        ad::ToArrayBasedExpression(f.GetExpression(), array);

        //check if the problem changed

        if (!this->gpu_initialized) {

            std::ifstream in;
            in.open("grad_calc2.cl");


            std::stringstream ss;
            std::string line = "";

            while (in.good()) {
                std::getline(in, line);
                ss << line << "\n";
            }

            this->opencl_kernel = ss.str();


            this->expression_size = 0;
            cl_int err = CL_SUCCESS;
            try {

                // Query platforms
                std::vector<cl::Platform> platforms;

                cl::Platform::get(&platforms);
                if (platforms.size() == 0) {
                    std::cout << "Platform size 0\n";
                    exit(0);
                }


                std::cout << platforms[0];

                // Get list of devices on default platform and create context
                cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties) (platforms[0])(), 0};
                context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
                devices = context.getInfo<CL_CONTEXT_DEVICES > ();
                //std::cout<<devices[0]<<"\n";

                const cl::Device device = devices[0];
                this->gpu_max_memory = device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE > (NULL);



                // Create command queue for first device
                this->queue = cl::CommandQueue(context, devices[0], 0, &err);



                //Build kernel from source string
                cl::Program::Sources source(1,
                        std::make_pair(this->opencl_kernel.c_str(), this->opencl_kernel.size()));
                program_ = cl::Program(context, source);
                program_.build(devices);

                // Create kernel object
                kernel = cl::Kernel(program_, "gradient", &err);



            } catch (cl::Error err) {
                std::cerr
                        << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
                std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
                exit(0);
            }
            this->gpu_initialized = true;




        }


        ad::ToArrayBasedExpression(f.GetExpression(), array);
        std::cout << "Array size = " << array.size << std::endl;
        size_t problem_size = array.Bytes() + parameter_ids.size()*(sizeof (int) *2 + sizeof (T)*2);

        // std::cout<<"using "<<problem_size<<" of "<<this->gpu_max_memory<<"("<<((float)problem_size/this->gpu_max_memory)<<"%) gpu memory\n";
        if ((problem_size) >= this->gpu_max_memory) {
            std::cout << "problem too big...." << std::endl;
            exit(0);
        }
        //        std::cout<<"-->"<<parameter_ids.size()*(sizeof (int) *2 + sizeof (T)*2);
        //           exit(0);
        size_t active_kernels = (this->gpu_max_memory / problem_size) - 1;
        //std::cout << "active kernels = " << active_kernels << "\n";

        //if (this->expression_size != array.size) {//reallocate
        try {
            this->expression_size = array.size;
            this->h_id = array.id.data();
            this->h_op = array.op.data();
            this->h_value = array.value.data();
            this->h_left = array.left.data();
            this->h_right = array.right.data();
            this->h_parameters = parameter_ids.data();


            //this->h_parameters = std::vector<int>(active_kernels * array.size);
            ////std::cout << __LINE__ << std::endl;
            d_id = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.id.size() * sizeof (int), h_id, 0);
            ////std::cout << __LINE__ << std::endl;
            d_op = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.op.size() * sizeof (int), h_op, 0);
            ////std::cout << __LINE__ << std::endl;
            d_value = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.value.size() * sizeof (HOST_TYPE), h_value, 0);
            ////std::cout << __LINE__ << std::endl;
            d_left = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.left.size() * sizeof (int), h_left, 0);
            ////std::cout << __LINE__ << std::endl;
            d_right = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, array.right.size() * sizeof (int), h_right, 0);
            ////std::cout << __LINE__ << std::endl;
            d_parameters = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, parameter_ids.size() * sizeof (int), h_parameters, 0);
            ////std::cout << __LINE__ << std::endl;
            d_gradient = cl::Buffer(context, CL_MEM_WRITE_ONLY, parameter_ids.size() * sizeof (HOST_TYPE));
            ////std::cout << __LINE__ << std::endl;
            d_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (int));
            ////std::cout << __LINE__ << std::endl;
            d_search_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (int));
            ////std::cout << __LINE__ << std::endl;
            d_value_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (HOST_TYPE));
            ////std::cout << __LINE__ << std::endl;
            d_derivative_stack = cl::Buffer(context, CL_MEM_READ_WRITE, array.size * parameter_ids.size() * sizeof (HOST_TYPE));

        } catch (cl::Error err) {
            std::cerr
                    << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
            std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
            exit(0);
        }
        //  }

        try {
            //            this->expression_size = array.size;
            //            this->h_id = array.id.data();
            //            this->h_op = array.op.data();
            //            this->h_value = array.value.data();
            //            this->h_left = array.left.data();
            //            this->h_right = array.right.data();
            //            this->h_parameters = parameter_ids.data();
            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_id, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (int));
            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_op, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (int));
            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_value, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (HOST_TYPE));
            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_left, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (int));
            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_right, CL_FALSE, CL_MAP_READ, 0, array.size * sizeof (int));
            //std::cout << __LINE__ << std::endl;
            queue.enqueueMapBuffer(d_parameters, CL_FALSE, CL_MAP_READ, 0, parameter_ids.size() * sizeof (int));
            //queue.enqueueMapBuffer(d_gradient, CL_TRUE, CL_MAP_WRITE, 0, parameter_ids.size() * sizeof (HOST_TYPE));

            kernel.setArg(0, d_id);
            kernel.setArg(1, d_op);
            kernel.setArg(2, d_value);
            kernel.setArg(3, d_left);
            kernel.setArg(4, d_right);

            int a = array.size;
            kernel.setArg(5, a);
            kernel.setArg(6, d_parameters);
            kernel.setArg(7, d_gradient);
            int p = parameter_ids.size();
            kernel.setArg(8, p);

            kernel.setArg(9, /* array.size  * sizeof (int), NULL); //*/d_stack);
            kernel.setArg(10, /*array.size * sizeof (int), NULL); //*/d_search_stack);
            kernel.setArg(11, /*array.size * sizeof (HOST_TYPE), NULL); //*/d_value_stack);
            kernel.setArg(12, /*array.size * sizeof (HOST_TYPE), NULL); //*/d_derivative_stack);

            unsigned int n = parameter_ids.size();
            // Number of work items in each local work group
            cl::NDRange localSize(1);
            // Number of total work items - localSize must be devisor
            cl::NDRange globalSize((int) (ceil(n / (HOST_TYPE) 64)*64));
            // std::cout<<(int) (ceil(n / (HOST_TYPE) 64)*64)<<"\n";
            //std::cout << "launching opencl kernels....\n";
            // Enqueue kernel
            cl::Event event;
            queue.enqueueNDRangeKernel(
                    kernel,
                    cl::NullRange,
                    globalSize,
                    localSize,
                    NULL,
                    &event);

            // Block until kernel completion
            event.wait();
            //            cl_ulong start =
            //                    event.getProfilingInfo<CL_PROFILING_COMMAND_START > ();
            //            cl_ulong end =
            //                    event.getProfilingInfo<CL_PROFILING_COMMAND_END > ();
            //            double time = 1.e-9 * (end - start);
            //            std::cout << "Time for kernel to execute " << time << std::endl;

            // Read back d_c
            queue.enqueueReadBuffer(d_gradient, CL_TRUE, 0, parameter_ids.size() * sizeof (HOST_TYPE), gradient.data());
            //
            //        for (int i = 0; i < n; i++) {
            //            gradient[i] = gg[i];
            //            // std::cout<<gg[i] << " ";
            //            //this->gradient_m[i] = gradient[i];
            //        }
        } catch (cl::Error err) {
            std::cerr
                    << "ERROR: " << err.what() << "(" << err.err() << ")" << std::endl;
            std::cout << "---> " << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG > (devices[0]);
            exit(0);
        }


    }
#endif
    
    void GradientCPU(const ad::ADNumber<T> &f,
            std::vector<int> &parameter_ids,
            std::vector<T> &gradient) {

        //        ad::ToArrayBasedExpression(f.GetExpression(), this->array);
        //
        //        this->gradient_cpu_p(array.id.data(), array.op.data(), array.value.data(), array.left.data(), array.right.data(), array.size, parameter_ids.data(), gradient.data(), parameter_ids.size());


        ad::PostOrderIterator<T> it(f.GetExpression());
        std::vector<CExp> cxep;
        while (it) {

            ad::Expression<T> * exp = (ad::Expression<T>*)it;
            CExp e;
            e.id = exp->GetId();
            e.op = exp->GetOp();
            e.value = exp->GetValue();
            cxep.push_back(e);

            it++;
        }

        this->gradient_(cxep, parameter_ids, gradient);


    }

    void GradientConcurrentCPU(const ad::ADNumber<T> &f,
            const std::vector<unsigned int> &parameter_ids,
            std::vector<T> &gradient) {

    }

private:

    /**
     * Evaluates an expression for the derivative with respect to wrt using a
     * postorder array representation of the Expression exp.
     * 
     * @param exp
     * @param wrt
     * @return 
     */
    const T derivative(std::vector<CExp> &expression, int wrt) {
        std::vector<T> values(expression.size());
        std::vector<T> derivatives(expression.size());
        int found, j;
        int vindex = -1;
        int dindex = -1;

        for (int i = 0; i < expression.size(); i++) {
            T lhs, rhs, dlhs, drhs, temp;
            found = 0;

            switch (expression[i].op) {

                case ad::CONSTANT:
                    values[++vindex] = expression[i].value;
                    derivatives[++dindex] = 0;
                    break;
                case ad::VARIABLE:

                    if (expression[i].id == wrt) {
                        //f(x) = x
                        //f'(x) = 1
                        values[++vindex] = expression[i].value;
                        derivatives[++dindex] = 1;
                    } else {//constant
                        //f(x) = C
                        //f'(x) = 0
                        values[++vindex] = expression[i].value;
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::PLUS:
                    rhs = values[vindex--];
                    lhs = values[vindex--];
                    drhs = derivatives[dindex--];
                    dlhs = derivatives[dindex--];
                    values[++vindex] = lhs + rhs;
                    derivatives[++dindex] = dlhs + drhs;
                    break;
                case ad::MINUS:

                    rhs = values[vindex--];
                    lhs = values[vindex--];
                    drhs = derivatives[dindex--];
                    dlhs = derivatives[dindex--];
                    values[++vindex] = lhs - rhs;
                    derivatives[++dindex] = dlhs - drhs;

                    break;
                case ad::MULTIPLY:
                    rhs = values[vindex--];
                    lhs = values[vindex--];
                    drhs = derivatives[dindex--];
                    dlhs = derivatives[dindex--];

                    temp = dlhs * rhs + lhs * drhs;
                    values[++vindex] = lhs * rhs;
                    derivatives[++dindex] = temp;
                    break;
                case ad::DIVIDE:
                    rhs = values[vindex--];
                    lhs = values[vindex--];
                    drhs = derivatives[dindex--];
                    dlhs = derivatives[dindex--];
                    temp = (dlhs * rhs + lhs * drhs) / (rhs * rhs);
                    values[++vindex] = lhs / rhs;
                    derivatives[++dindex] = temp;
                    break;
                case ad::SIN:

                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];
                    // std::cout<<"sin "<<lhs<<" "<<dlhs;
                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        values[++vindex] = sin((T) lhs);
                        derivatives[++dindex] = dlhs * cos((T) lhs);
                        // std::cout<<" = "<<dlhs * cos((T) lhs)<<" == "<<derivatives[i]<<"\n";
                    } else {
                        values[++vindex] = sin((T) lhs);
                        derivatives[++dindex] = dlhs * cos((T) lhs);
                    }
                    break;
                case ad::COS:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        values[++vindex] = cos(lhs);
                        derivatives[++dindex] = dlhs *-1.0 * sin((T) lhs);
                    } else {
                        values[++vindex] = cos(lhs);
                        derivatives[++dindex] = dlhs *-1.0 * sin((T) lhs);
                    }
                    break;
                case ad::TAN:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = dlhs * (((1.0) / cos((T) lhs))*((1.0) / cos((T) lhs)));
                        values[++vindex] = tan((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = tan((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::ASIN:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * (1.0) / pow((T) ((1.0) - pow((T) lhs, (2.0))), (0.5)));
                        values[++vindex] = asin((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = asin((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::ACOS:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * (-1.0) / pow((T) ((1.0) - pow((T) lhs, (2.0))), (0.5)));
                        values[++vindex] = acos((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = acos((T) lhs);
                        derivatives[++dindex] = 0;
                    }

                    break;
                case ad::ATAN:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * (1.0) / (lhs * lhs + (1.0)));
                        values[++vindex] = atan((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = atan((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::ATAN2:
                    rhs = values[vindex--];
                    lhs = values[vindex--];
                    drhs = derivatives[dindex--];
                    dlhs = derivatives[dindex--];
                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (rhs * dlhs / (lhs * lhs + (rhs * rhs)));
                        values[++vindex] = atan2((T) lhs, (T) rhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = atan2((T) lhs, (T) rhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::SQRT:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = dlhs * (.5) / sqrt((T) lhs);
                        values[++vindex] = sqrt((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = sqrt(lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::POW:
                    rhs = values[vindex--];
                    lhs = values[vindex--];
                    drhs = derivatives[dindex--];
                    dlhs = derivatives[dindex--];
                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * rhs) *
                                pow(lhs, (rhs - (1.0)));
                        values[++vindex] = pow((T) lhs, (T) rhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = pow((T) lhs, (T) rhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::LOG:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * (1.0)) / lhs;
                        values[++vindex] = log((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = log((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::LOG10:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * (1.0)) / (lhs * log((T) (10.0)));
                        values[++vindex] = log10((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = log10((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::EXP:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = dlhs * exp((T) lhs);
                        values[++vindex] = exp((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = exp((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::SINH:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = dlhs * cosh((T) lhs);
                        values[++vindex] = sinh((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = sinh((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::COSH:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = dlhs * sinh((T) lhs);
                        values[++vindex] = cosh((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = cosh((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::TANH:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = dlhs * ((1.0) / cosh((T) lhs))*((1.0) / cosh((T) lhs));
                        values[++vindex] = tanh((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = tanh((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::FABS:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * lhs) /
                                fabs(lhs);
                        values[++vindex] = fabs((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = fabs((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::ABS:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {
                        temp = (dlhs * lhs) /
                                fabs(lhs);
                        values[++vindex] = fabs((T) lhs);
                        derivatives[++dindex] = temp;
                    } else {
                        values[++vindex] = fabs((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::FLOOR:
                    lhs = values[vindex--];
                    dlhs = derivatives[dindex--];

                    for (j = i; j>-1; j--) {
                        if (expression[j].id == wrt) {
                            found = 1;
                            break;
                        }
                    }

                    if (found) {

                        values[++vindex] = floor((T) lhs);
                        derivatives[++dindex] = 0;
                    } else {
                        values[++vindex] = floor((T) lhs);
                        derivatives[++dindex] = 0;
                    }
                    break;
                case ad::NONE:
                    break;
                default:
                    break;

            }



        }


        return derivatives[0];

    }

    void gradient_(std::vector<CExp> &expr, std::vector<int> &p, std::vector<T> &gradient) {
        //        std::vector<T> values(expression.size());
        //        std::vector<T> derivatives(expression.size());
        //std::cout<<"starting grading calcs....\n";
        size_t e_size = expr.size();
        size_t p_size = p.size();
        if (this->value_stack.size() != e_size) {
            this->value_stack = std::vector<T > (e_size);
            this->derivative_stack = std::vector<T > (e_size);
        }

        this->max_gradient_component =0;

        T* values = (value_stack.size() != 0) ? &value_stack.front() : NULL;
        T* derivatives = (derivative_stack.size() != 0) ? &derivative_stack.front() : NULL;
        CExp* expression = (e_size != 0) ? &expr.front() : NULL;

        int g;
        int i;
        for (g = 0; g < p_size; g++) {

            int wrt = p[g];
            int found, j;
            int vindex = -1;
            int dindex = -1;
            found = 0;


            for (i = 0; i < e_size; i++) {

                T lhs, rhs, dlhs, drhs, temp;


                switch (expression[i].op) {

                    case ad::CONSTANT:
                        values[++vindex] = expression[i].value;
                        derivatives[++dindex] = 0;
                        break;
                    case ad::VARIABLE:

                        if (expression[i].id == wrt) {
                            found = 1;
                            //f(x) = x
                            //f'(x) = 1
                            values[++vindex] = expression[i].value;
                            derivatives[++dindex] = 1;
                        } else {//constant
                            //f(x) = C
                            //f'(x) = 0
                            values[++vindex] = expression[i].value;
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::PLUS:
                        rhs = values[vindex--];
                        lhs = values[vindex--];
                        drhs = derivatives[dindex--];
                        dlhs = derivatives[dindex--];
                        values[++vindex] = lhs + rhs;
                        derivatives[++dindex] = dlhs + drhs;
                        break;
                    case ad::MINUS:

                        rhs = values[vindex--];
                        lhs = values[vindex--];
                        drhs = derivatives[dindex--];
                        dlhs = derivatives[dindex--];
                        values[++vindex] = lhs - rhs;
                        derivatives[++dindex] = dlhs - drhs;

                        break;
                    case ad::MULTIPLY:
                        rhs = values[vindex--];
                        lhs = values[vindex--];
                        drhs = derivatives[dindex--];
                        dlhs = derivatives[dindex--];

                        temp = dlhs * rhs + lhs * drhs;
                        values[++vindex] = lhs * rhs;
                        derivatives[++dindex] = temp;
                        break;
                    case ad::DIVIDE:
                        rhs = values[vindex--];
                        lhs = values[vindex--];
                        drhs = derivatives[dindex--];
                        dlhs = derivatives[dindex--];
                        temp = (dlhs * rhs - lhs * drhs) / (rhs * rhs);
                        values[++vindex] = lhs / rhs;
                        derivatives[++dindex] = temp;
                        break;
                    case ad::SIN:

                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];
                        //                        // std::cout<<"sin "<<lhs<<" "<<dlhs;
                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            values[++vindex] = std::sin((T) lhs);
                            derivatives[++dindex] = dlhs * std::cos((T) lhs);
                            // std::cout<<" = "<<dlhs * cos((T) lhs)<<" == "<<derivatives[i]<<"\n";
                        } else {
                            values[++vindex] = std::sin((T) lhs);
                            derivatives[++dindex] = dlhs * std::cos((T) lhs);
                        }
                        break;
                    case ad::COS:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            values[++vindex] = std::cos(lhs);
                            derivatives[++dindex] = dlhs *-1.0 * std::sin((T) lhs);
                        } else {
                            values[++vindex] = std::cos(lhs);
                            derivatives[++dindex] = dlhs *-1.0 * std::sin((T) lhs);
                        }
                        break;
                    case ad::TAN:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = dlhs * (((1.0) / std::cos((T) lhs))*((1.0) / std::cos((T) lhs)));
                            values[++vindex] = std::tan((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::tan((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::ASIN:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * (1.0) / std::pow((T) ((1.0) - std::pow((T) lhs, (2.0))), (0.5)));
                            values[++vindex] = std::asin((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::asin((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::ACOS:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * (-1.0) / std::pow((T) ((1.0) - std::pow((T) lhs, (2.0))), (0.5)));
                            values[++vindex] = std::acos((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::acos((T) lhs);
                            derivatives[++dindex] = 0;
                        }

                        break;
                    case ad::ATAN:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * (1.0) / (lhs * lhs + (1.0)));
                            values[++vindex] = std::atan((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::atan((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::ATAN2:
                        rhs = values[vindex--];
                        lhs = values[vindex--];
                        drhs = derivatives[dindex--];
                        dlhs = derivatives[dindex--];
                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (rhs * dlhs / (lhs * lhs + (rhs * rhs)));
                            values[++vindex] = std::atan2((T) lhs, (T) rhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::atan2((T) lhs, (T) rhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::SQRT:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = dlhs * (.5) / std::sqrt((T) lhs);
                            values[++vindex] = std::sqrt((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::sqrt(lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::POW:
                        rhs = values[vindex--];
                        lhs = values[vindex--];
                        drhs = derivatives[dindex--];
                        dlhs = derivatives[dindex--];
                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * rhs) *
                                    std::pow(lhs, (rhs - (1.0)));
                            values[++vindex] = std::pow((T) lhs, (T) rhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::pow((T) lhs, (T) rhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::LOG:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * (1.0)) / lhs;
                            values[++vindex] = std::log((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::log((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::LOG10:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * (1.0)) / (lhs * std::log((T) (10.0)));
                            values[++vindex] = std::log10((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::log10((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::EXP:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = dlhs * exp((T) lhs);
                            values[++vindex] = std::exp((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::exp((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::SINH:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = dlhs * std::cosh((T) lhs);
                            values[++vindex] = std::sinh((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::sinh((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::COSH:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = dlhs * std::sinh((T) lhs);
                            values[++vindex] = std::cosh((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::cosh((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::TANH:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = dlhs * ((1.0) / std::cosh((T) lhs))*((1.0) / std::cosh((T) lhs));
                            values[++vindex] = std::tanh((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::tanh((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::FABS:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * lhs) /
                                    std::fabs(lhs);
                            values[++vindex] = std::fabs((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = fabs((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::ABS:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {
                            temp = (dlhs * lhs) /
                                    fabs(lhs);
                            values[++vindex] = std::fabs((T) lhs);
                            derivatives[++dindex] = temp;
                        } else {
                            values[++vindex] = std::fabs((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::FLOOR:
                        lhs = values[vindex--];
                        dlhs = derivatives[dindex--];

                        //                        for (j = i; j>-1; j--) {
                        //                            if (expression[j].id == wrt) {
                        //                                found = 1;
                        //                                break;
                        //                            }
                        //                        }

                        if (found) {

                            values[++vindex] = std::floor((T) lhs);
                            derivatives[++dindex] = 0;
                        } else {
                            values[++vindex] = std::floor((T) lhs);
                            derivatives[++dindex] = 0;
                        }
                        break;
                    case ad::NONE:
                        break;
                    default:
                        break;

                }



            }

            if(std::fabs(derivatives[0])> std::fabs(this->max_gradient_component)){
                this->max_gradient_component = derivatives[0];
            }
            
           
            gradient[g] = derivatives[0];
        }
    }

    int has_id_p(int *id, int*left, int *right, int root, int has, int size) {

        int index = -1;
        int stack[size];
        int found = 0;
        int node = root;

        while (index != -1 || node != -999) {
            if (node != -999) {
                stack[++index] = node;
                node = left[node];

            } else {
                node = stack[index--];
                if (id[node] == has) {
                    return 1;
                }
                node = right[node];
            }
        }

        return found;
    }

    void gradient_cpu_p(int *id,
            int*op,
            T* value,
            int*left,
            int *right,
            int size,
            unsigned int*parameters,
            T* gradient,
            int gradient_size) {



        int stack[size];
        T vstack[size];
        T dstack[size];

        for (int i = 0; i < gradient_size; i++) {
            int wrt = parameters[i];
            int index = -1;
            int vindex = -1;
            int dindex = -1;


            stack[++index] = 0;
            int prevNode = -999;
            int currNode = -999;

            while (index>-1) {
                currNode = stack[index];


                if (prevNode == -999 || left[prevNode] == currNode || right[prevNode] == currNode) {
                    if (left[currNode] != -999) {
                        stack[++index] = left[currNode];
                    } else if (right[currNode] != -999) {
                        stack[++index] = right[currNode];
                    }
                } else if (left[currNode] == prevNode) {
                    if (right[currNode] != -999) {
                        stack[++index] = right[currNode];
                    }
                } else {

                    T lhs = 0;
                    T rhs = 0;
                    T dlhs = 0;
                    T drhs = 0;
                    T temp = 0;

                    switch (op[currNode]) {

                        case ad::CONSTANT:
                            vstack[++vindex] = value[currNode];
                            dstack[++dindex] = 0;
                            break;
                        case ad::VARIABLE:
                            if (id[currNode] == wrt) {
                                //f(x) = x
                                //f'(x) = 1
                                vstack[++vindex] = value[currNode];
                                dstack[++dindex] = 1;
                            } else {//constant
                                //f(x) = C
                                //f'(x) = 0
                                vstack[++vindex] = value[currNode];
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::PLUS:
                            rhs = vstack[vindex--];
                            lhs = vstack[vindex--];
                            drhs = dstack[dindex--];
                            dlhs = dstack[dindex--];
                            vstack[++vindex] = lhs + rhs;
                            dstack[++dindex] = dlhs + drhs;
                            break;
                        case ad::MINUS:
                            rhs = vstack[vindex--];
                            lhs = vstack[vindex--];
                            drhs = dstack[dindex--];
                            dlhs = dstack[dindex--];
                            vstack[++vindex] = lhs - rhs;
                            dstack[++dindex] = dlhs - drhs;
                            break;
                        case ad::MULTIPLY:
                            rhs = vstack[vindex--];
                            lhs = vstack[vindex--];
                            drhs = dstack[dindex--];
                            dlhs = dstack[dindex--];
                            temp = dlhs * rhs + lhs * drhs;
                            vstack[++vindex] = lhs*rhs;
                            dstack[++dindex] = temp;
                            break;
                        case ad::DIVIDE:
                            rhs = vstack[vindex--];
                            lhs = vstack[vindex--];
                            drhs = dstack[dindex--];
                            dlhs = dstack[dindex--];
                            temp = (dlhs * rhs - lhs * drhs) / (rhs * rhs);
                            vstack[++vindex] = lhs / rhs;
                            dstack[++dindex] = temp;
                            break;

                        case ad::SIN:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                vstack[++vindex] = sin((T) lhs);
                                dstack[++dindex] = dlhs * cos((T) lhs);
                            } else {
                                vstack[++vindex] = sin((T) lhs);
                                dstack[++dindex] = dlhs * cos((T) lhs);
                            }
                            break;
                        case ad::COS:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                vstack[++vindex] = cos(lhs);
                                dstack[++dindex] = dlhs *-1.0f * sin((T) lhs);
                            } else {
                                vstack[++vindex] = cos(lhs);
                                dstack[++dindex] = dlhs *-1.0f * sin((T) lhs);
                            }
                            break;
                        case ad::TAN:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = dlhs * (((1.0f) / cos((T) lhs))*((1.0f) / cos((T) lhs)));
                                vstack[++vindex] = tan((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = tan((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::ASIN:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * (1.0f) / pow((T) ((1.0f) - pow((T) lhs, (2.0f))), (0.5f)));
                                vstack[++vindex] = asin((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = asin((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::ACOS:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * (-1.0f) / pow((T) ((1.0f) - pow((T) lhs, (2.0f))), (0.5f)));
                                vstack[++vindex] = acos((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = acos((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::ATAN:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * (1.0f) / (lhs * lhs + (1.0f)));
                                vstack[++vindex] = atan((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = atan((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::ATAN2:
                            rhs = vstack[vindex--];
                            lhs = vstack[vindex--];
                            drhs = dstack[dindex--];
                            dlhs = dstack[dindex--];
                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (rhs * dlhs / (lhs * lhs + (rhs * rhs)));
                                vstack[++vindex] = atan2((T) lhs, (T) rhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = atan2((T) lhs, (T) rhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::SQRT:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = dlhs * (.5f) / sqrt((T) lhs);
                                vstack[++vindex] = sqrt((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = sqrt(lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::POW:
                            rhs = vstack[vindex--];
                            lhs = vstack[vindex--];
                            drhs = dstack[dindex--];
                            dlhs = dstack[dindex--];
                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * rhs) *
                                        pow(lhs, (rhs - (1.0f)));
                                vstack[++vindex] = pow((T) lhs, (T) rhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = pow((T) lhs, (T) rhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::LOG:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * (1.0f)) / lhs;
                                vstack[++vindex] = log((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = log((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::LOG10:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * (1.0f)) / (lhs * log((T) (10.0f)));
                                vstack[++vindex] = log10((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = log10((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::EXP:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = dlhs * exp((T) lhs);
                                vstack[++vindex] = exp((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = exp((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::SINH:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = dlhs * cosh((T) lhs);
                                vstack[++vindex] = sinh((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = sinh((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::COSH:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = dlhs * sinh((T) lhs);
                                vstack[++vindex] = cosh((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = cosh((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::TANH:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = dlhs * ((1.0f) / cosh((T) lhs))*((1.0f) / cosh((T) lhs));
                                vstack[++vindex] = tanh((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = tanh((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::FABS:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * lhs) /
                                        fabs(lhs);
                                vstack[++vindex] = fabs((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = fabs((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::ABS:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {
                                temp = (dlhs * lhs) /
                                        fabs((T) lhs);
                                vstack[++vindex] = fabs((T) lhs);
                                dstack[++dindex] = temp;
                            } else {
                                vstack[++vindex] = fabs((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::FLOOR:
                            lhs = vstack[vindex--];
                            dlhs = dstack[dindex--];

                            if (has_id_p(id, left, right, currNode, wrt, size)) {

                                vstack[++vindex] = floor((T) lhs);
                                dstack[++dindex] = 0;
                            } else {
                                vstack[++vindex] = floor((T) lhs);
                                dstack[++dindex] = 0;
                            }
                            break;
                        case ad::NONE:
                            break;
                        default:
                            break;

                    }

                    index--;
                }

                prevNode = currNode;

            }

            gradient[i] = dstack[0];

        }
    }

};



#endif	/* GRADIENTCALCULATOR_HPP */

