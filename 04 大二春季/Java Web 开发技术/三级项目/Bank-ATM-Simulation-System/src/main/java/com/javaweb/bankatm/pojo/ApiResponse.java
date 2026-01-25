package com.javaweb.bankatm.pojo;

import org.springframework.http.HttpStatus;

/**
 * @Classname ApiResponce
 * @Description 包装相应信息的Bean
 * @Date 2025/3/17 上午4:38
 * @Created by Lee
 */
public class ApiResponse {

    private int status;         // HTTP状态码
    private String message;     // 错误信息或者成功消息
    private Object data;        // 额外的数据（通常是查询结果、列表、对象等）

    // 构造函数 - 用于正常的响应
    public ApiResponse(HttpStatus status, String message, Object data) {
        this.status = status.value();
        this.message = message;
        this.data = data;
    }

    // 构造函数 - 用于错误响应
    public ApiResponse(HttpStatus status, String message) {
        this.status = status.value();
        this.message = message;
        this.data = null;  // 没有数据时将其置为 null
    }

    // getter 和 setter 方法

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public Object getData() {
        return data;
    }

    public void setData(Object data) {
        this.data = data;
    }
}

