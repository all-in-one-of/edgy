// todo
//  custom particle sizes
//  aligning strokes to geo

#include <ai.h>
 
AI_SHADER_NODE_EXPORT_METHODS(edgeMethods);

struct ShaderData {
  AtString trace_set;
  AtVector cam_pos;
};


enum EdgeParams {
  p_pos,
  p_dir,
  p_cam_pos
};
 
node_parameters {
  AiParameterVec("pos", 0.0, 0.0, 0.0);
  AiParameterVec("dir", 0.0, 0.0, 0.0);
  AiParameterVec("cam_pos", 0.0, 0.0, 0.0);
  AiParameterStr("traceset", "");
}

node_initialize {
  ShaderData* data = new ShaderData;
  AiNodeSetLocalData(node, data);
}

node_update {
  ShaderData *data = (ShaderData*)AiNodeGetLocalData(node);
  data->trace_set = AiNodeGetStr(node, "traceset");
  data->cam_pos = AiNodeGetVec(node, "cam_pos");
}

node_finish {
  ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);
  AiNodeSetLocalData(node, NULL);
  delete data;
}
 
shader_evaluate {
  ShaderData* data = (ShaderData*)AiNodeGetLocalData(node);

  AtVector pos = AiShaderEvalParamVec(p_pos);
  AtVector dir = AiShaderEvalParamVec(p_dir);
  
  if (data->trace_set.length()) {
    AiShaderGlobalsSetTraceSet(sg, data->trace_set, false);
  }

  AtRay ray_intersect_along_normal = AiMakeRay(AI_RAY_CAMERA, pos, &dir, AI_BIG, sg);
  AtScrSample hit = AtScrSample();

  if(AiTrace(ray_intersect_along_normal, AI_RGB_WHITE, hit)){
    AtVector hitpoint = hit.point;
    AtVector dir_cam_hit = AiV3Normalize(hitpoint - data->cam_pos);

    AtRay ray_intersect_camera = AiMakeRay(AI_RAY_CAMERA, data->cam_pos, &dir_cam_hit, AI_BIG, sg);
    AtScrSample hit2 = AtScrSample();

    if(AiTrace(ray_intersect_camera, AI_RGB_WHITE, hit2)){
      sg->out.RGB() = hit2.color;
    }
  }

  AiShaderGlobalsUnsetTraceSet(sg);
}
 
node_loader {
   if (i > 0) return false;
   node->methods     = edgeMethods;
   node->output_type = AI_TYPE_RGB;
   node->name        = "edge";
   node->node_type   = AI_NODE_SHADER;
   strcpy(node->version, AI_VERSION);
   return true;
}