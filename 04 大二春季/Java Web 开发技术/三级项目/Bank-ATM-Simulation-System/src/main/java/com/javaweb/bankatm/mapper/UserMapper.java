package com.javaweb.bankatm.mapper;

import com.javaweb.bankatm.pojo.User;
import org.apache.ibatis.annotations.*;

import java.util.List;

/**
 * @Classname UserMapper
 * @Description user_tb表的操作映射
 * @Date 2025/3/17 上午3:45
 * @Created by Lee
 */
@Mapper
public interface UserMapper {
    @Select("select * from user_tb where user_name != 'root'")
    public List<User> getUserList();

    @Select("select * from user_tb where card_number = #{card_number} and password = #{password}")
    public User findByCardAndPassword(@Param("card_number")String cardNumber, @Param("password")String password);

    @Select("select * from user_tb where card_number = #{card_number}")
    public User findByCardNumber(@Param("card_number")String cardNumber);

    @Select("select max(user_id) from user_tb")
    Integer getMaxUserId();

    @Insert("insert into user_tb (user_id, user_name, card_number, password, balance, created_at) " +
            "values (#{user_id}, #{user.userName}, #{user.cardNumber}, #{user.password}, #{user.balance}, #{user.createdAt})")
    public Boolean addUserInfo(@Param("user_id")Integer user_id, @Param("user")User user);

    @Delete("delete from user_tb where card_number = #{card_number} and password = #{password}")
    public Boolean deleteByCardAndPassword(@Param("card_number")String cardNumber, @Param("password")String password);

    @Select("select balance from user_tb where card_number = #{card_number}")
    public Double getBalanceByCardNumber(@Param("card_number")String cardNumber);

    @Update("update user_tb set balance = balance + #{amount} where card_number = #{card_number}")
    public Boolean depositByCardNumber(@Param("card_number")String cardNumber, @Param("amount")Double amount);

    @Update("update user_tb set balance = balance - #{amount} where card_number = #{card_number}")
    public Boolean withdrawByCardNumber(@Param("card_number")String cardNumber, @Param("amount")Double amount);

    @Select("select user_name from user_tb where card_number = #{card_number}")
    public String findUserNameByCardNumber(@Param("card_number")String cardNumber);

    @Update("update user_tb set user_name = #{user_name_new}, card_number = #{card_number_new}, password = #{password_new} where card_number = #{card_number}")
    public Boolean changeUserInfo(@Param("card_number") String cardNumber, @Param("user_name_new") String userNameNew, @Param("card_number_new") String cardNumberNew, @Param("password_new") String passwordNew);
}