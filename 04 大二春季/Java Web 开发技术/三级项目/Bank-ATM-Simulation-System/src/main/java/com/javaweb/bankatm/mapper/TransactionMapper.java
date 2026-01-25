package com.javaweb.bankatm.mapper;

import com.javaweb.bankatm.pojo.Transaction;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Param;
import org.apache.ibatis.annotations.Select;

import java.util.List;

/**
 * @Classname TransactionMapper
 * @Description transaction_tb表的操作映射
 * @Date 2025/3/17 上午3:45
 * @Created by Lee
 */
@Mapper
public interface TransactionMapper {
    @Select("select * from transaction_tb where user_name != 'root'")
    public List<Transaction> getTransactionAll();

    @Select("select * from transaction_tb where user_card_number = #{card_number}")
    public List<Transaction> getTransactionByCardNumber(@Param("card_number")String cardNumber);

    @Select("select max(transaction_id) from transaction_tb")
    public Integer getMaxTransactionId();

    @Insert("insert into transaction_tb (transaction_id, user_name, user_card_number, type, amount, " +
            "target_user_name, target_user_card_number, timestamp, transaction_status)" +
            "values (#{tran_id}, #{tran.userName}, #{tran.userCardNumber}, #{tran.type}, #{tran.amount}, " +
            "#{tran.targetUserName}, #{tran.targetUserCardNumber}, #{tran.timestamp}, #{tran.transactionStatus})")
    public void addTransactionInfo(@Param("tran_id")Integer tran_id, @Param("tran")Transaction transaction);
}