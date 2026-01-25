package com.javaweb.bankatm.service;

import com.javaweb.bankatm.pojo.ApiResponse;
import com.javaweb.bankatm.pojo.DTO.ChangeDTO;
import com.javaweb.bankatm.pojo.DTO.LoginDTO;
import com.javaweb.bankatm.pojo.DTO.RegisterDTO;
import com.javaweb.bankatm.pojo.User;
import jakarta.servlet.http.HttpSession;
import org.springframework.stereotype.Service;

/**
 * @Classname UserService
 * @Description UserService服务层的方法定义
 * @Date 2025/3/17 上午4:52
 * @Created by Lee
 */
public interface UserService {

    /**
     * 通过登录信息查找User
     * @param loginDTO
     * @return User
     */
    public ApiResponse login(LoginDTO loginDTO, HttpSession session);


    /**
     * 注册新用户
     * @param
     * @return Boolean
     */
    public ApiResponse registerUser(RegisterDTO userDTO);

    /**
     * 删除用户
     * @param
     * @return Boolean
     */
    public ApiResponse deleteUser(HttpSession session);

    /**
     * 修改用户信息
     * @param
     * @return
     */
    public ApiResponse changeUserInfo(ChangeDTO changeDTO, HttpSession session);

    public ApiResponse getUserAll();

    public ApiResponse getUser(String cardNumber);
}
