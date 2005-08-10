void ComputeColor(double val, double min, double max, GLfloat *color)
{
        GLfloat cval;
        
        if(min != max)
                cval = (val - min) / (max - min);
        else
                cval = 0.5;

        color[0] = 0.0;
        color[1] = 0.0;
        color[2] = 0.0;
        color[3] = 1.0;

        if(cval > 0.5)
        {
                color[0] = (cval - 0.5)*2;
                color[1] = (1.0 - cval)*2;
        }
        else
        {
                color[1] = (cval)*2;
                color[2] = (0.5 - cval)*2;
        }

}/*ComputeColor*/

